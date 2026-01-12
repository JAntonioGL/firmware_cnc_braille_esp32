/*
 * ESP32 Braille - DEMO por eventos + Debug "antes y después"
 * ----------------------------------------------------------
 * LEDS:
 *   LED_DIR  (GPIO33): ON=RTL, OFF=LTR (sentido de la fila)
 *   LED_SOL  (GPIO25): golpe (1)
 *   LED_SEP  (GPIO26): separación corta
 *   LED_ESP  (GPIO27): espacio mediano
 *   LED_LINE (GPIO14): salto de línea
 *
 * Protocolo Serial (115200):
 *   CELLS M N
 *   <M lineas de texto>
 *   END
 */

#include <Arduino.h>
#include "Braille.h"
#include "Translator.h"

static const unsigned long BAUD = 115200;

// ---- Pines LEDs ----
const uint8_t LED_DIR  = 33;
const uint8_t LED_SOL  = 25;
const uint8_t LED_SEP  = 26;
const uint8_t LED_ESP  = 27;
const uint8_t LED_LINE = 14;

// ---- Tiempos visibles ----
const uint16_t T_GOLPE_MS = 120;
const uint16_t T_SEP_MS   = 100;
const uint16_t T_ESP_MS   = 150;
const uint16_t T_LINE_MS  = 200;
const uint16_t T_ENTRE_EVENTOS_MS = 30;

// ---- Util ----
String rtrim(const String& s) {
  int end = s.length();
  while (end > 0 && (s[end-1] == '\r' || s[end-1] == '\n')) end--;
  return s.substring(0, end);
}
void blink(uint8_t pin, uint16_t t_on_ms) {
  digitalWrite(pin, HIGH);
  delay(t_on_ms);
  digitalWrite(pin, LOW);
  delay(T_ENTRE_EVENTOS_MS);
}

// ====== Solenoide (LED) ======
class Solenoid {
public:
  void begin(uint8_t pin) { _pin = pin; pinMode(_pin, OUTPUT); digitalWrite(_pin, LOW); }
  void pulse(uint16_t t_ms) { blink(_pin, t_ms); }
private:
  uint8_t _pin = 255;
};

// ====== Movimiento (LED eventos) ======
enum class MoveResult : uint8_t { OK, LIMIT_HIT, ERROR };

class Movement {
public:
  void begin(uint8_t ledDir, uint8_t ledSep, uint8_t ledEsp, uint8_t ledLine) {
    _ledDir = ledDir; _ledSep = ledSep; _ledEsp = ledEsp; _ledLine = ledLine;
    pinMode(_ledDir, OUTPUT);
    pinMode(_ledSep, OUTPUT);
    pinMode(_ledEsp, OUTPUT);
    pinMode(_ledLine, OUTPUT);
    digitalWrite(_ledDir, LOW);
    digitalWrite(_ledSep, LOW);
    digitalWrite(_ledEsp, LOW);
    digitalWrite(_ledLine, LOW);
  }
  // true=RTL (fila espejada), false=LTR
  void setZigzagDirection(bool rtl) {
    _rtl = rtl;
    digitalWrite(_ledDir, _rtl ? HIGH : LOW); // ON=RTL, OFF=LTR
  }
  MoveResult moveSepCorto()   { blink(_ledSep,  T_SEP_MS); return MoveResult::OK; }
  MoveResult moveEspMediano() { blink(_ledEsp,  T_ESP_MS); return MoveResult::OK; }
  MoveResult moveSaltoLinea() { blink(_ledLine, T_LINE_MS); return MoveResult::OK; }
private:
  uint8_t _ledDir=255, _ledSep=255, _ledEsp=255, _ledLine=255;
  bool _rtl=false;
};

// ====== Impresor ======
struct PrintParams {
  uint16_t t_golpe_ms = T_GOLPE_MS;
  bool retornoAlMargen = true;
};

class Printer {
public:
  void begin(Movement* mov, Solenoid* sol, PrintParams p) {
    _mov = mov; _sol = sol; _p = p;
  }

  // IMPORTANTE: Fila 0 = espejada (RTL), fila 1 = normal (LTR), y así alternando.
  // Es decir, arrancamos en RTL como pidió el usuario.
  void printMatrix(const uint8_t* braille, size_t rows, size_t cols) {
  if (!_mov || !_sol) return;

  Serial.println(F("[Printer] Inicio (lectura lineal; sentido lo fija Movimiento)"));
  for (size_t r=0; r<rows; ++r) {

    // Movimiento define el sentido por fila (p.ej., fila par = RTL, impar = LTR)
    // Si prefieres lo contrario, cambia la condición.
    bool rtlEstaFila = ((r % 2) == 0);     // fila 0 = derecha→izquierda
    _mov->setZigzagDirection(rtlEstaFila); // Printer NO invierte datos, solo informa fila

    Serial.print(F("  - Fila ")); Serial.print(r);
    Serial.print(F(" (")); Serial.print(rtlEstaFila ? F("RTL") : F("LTR"));
    Serial.println(F(")"));

    uint16_t puntosEnCelda = 0;

    // Lectura SIEMPRE de izquierda a derecha en memoria
    for (size_t c=0; c<cols; ++c) {
      uint8_t bit = braille[r*cols + c];

      // Solo decide golpe/no golpe; no calcula direcciones
      if (bit) _sol->pulse(_p.t_golpe_ms);

      // Avance corto entre puntos del mismo símbolo
      _mov->moveSepCorto();

      // Cada 2 columnas termina una celda Braille → espacio mediano
      puntosEnCelda++;
      if (puntosEnCelda >= 2) {
        _mov->moveEspMediano();
        puntosEnCelda = 0;
      }
    }

    // Fin de fila → salto de línea (y el módulo de movimiento ya sabe el retorno)
    if (_mov->moveSaltoLinea() != MoveResult::OK) {
      Serial.println(F("[Printer] ERROR en salto de línea."));
      break;
    }
  }
  Serial.println(F("[Printer] Terminado."));
}

private:
  Movement* _mov = nullptr;
  Solenoid* _sol = nullptr;
  PrintParams _p;

  void processDot(uint8_t bit, uint16_t& puntosEnCelda) {
    if (bit) _sol->pulse(_p.t_golpe_ms);  // golpe visible solo si hay 1
    _mov->moveSepCorto();                 // separación corta
    puntosEnCelda++;
    if (puntosEnCelda >= 2) {             // cada 2 columnas = fin de celda
      _mov->moveEspMediano();
      puntosEnCelda = 0;
    }
  }
};

// ====== Instancias ======
Solenoid  gSol;
Movement  gMov;
Printer   gPrn;
PrintParams gParams;

// ====== Cabecera ======
bool leerCabecera(size_t& M, size_t& N) {
  while (Serial.available() == 0) { delay(5); }
  String header = rtrim(Serial.readStringUntil('\n'));
  header.trim();
  if (!header.startsWith("CELLS")) return false;

  int i1 = header.indexOf(' ');
  int i2 = header.indexOf(' ', i1+1);
  if (i1 < 0 || i2 < 0) return false;
  String sM = header.substring(i1+1, i2);
  String sN = header.substring(i2+1);
  M = (size_t) sM.toInt();
  N = (size_t) sN.toInt();
  return (M > 0 && N > 0);
}

// ====== Debug helpers: “antes y después” ======
void imprimirMatrizBase(const uint8_t* braille, size_t rows, size_t cols) {
  Serial.println(F("== MATRIZ BASE (sin espejo) =="));
  for (size_t r=0; r<rows; ++r) {
    String line; line.reserve(cols + cols/2);
    for (size_t c=0; c<cols; ++c) {
      line += (braille[r*cols + c] ? '1' : '0');
      if ((c % 2)==1 && c+1 < cols) line += ' ';
    }
    Serial.print(F("f")); Serial.print(r); Serial.print(F(" → "));
    Serial.println(line);
  }
  Serial.println();
}

void imprimirVistaZigzag(const uint8_t* braille, size_t rows, size_t cols) {
  Serial.println(F("== VISTA ZIGZAG (así se recorre) =="));
  for (size_t r=0; r<rows; ++r) {
    bool espejo = (r % 2) == 0; // fila 0 RTL
    String line; line.reserve(cols + cols/2);
    if (!espejo) {
      for (size_t c=0; c<cols; ++c) {
        line += (braille[r*cols + c] ? '1' : '0');
        if ((c % 2)==1 && c+1 < cols) line += ' ';
      }
      Serial.print(F("f")); Serial.print(r); Serial.print(F(" → (LTR →) "));
      Serial.println(line);
    } else {
      for (int c=int(cols)-1; c>=0; --c) {
        line += (braille[r*cols + c] ? '1' : '0');
        if ((c % 2)==0 && c>0) line += ' '; // espacio cada 2 col también al invertir
      }
      Serial.print(F("f")); Serial.print(r); Serial.print(F(" ← (RTL) "));
      Serial.println(line);
    }
  }
  Serial.println();
}

// ====== Setup ======
void setup() {
  Serial.begin(BAUD);
  delay(300); // pequeño respiro para que el monitor serie alcance a enganchar

  pinMode(LED_DIR,  OUTPUT);
  pinMode(LED_SOL,  OUTPUT);
  pinMode(LED_SEP,  OUTPUT);
  pinMode(LED_ESP,  OUTPUT);
  pinMode(LED_LINE, OUTPUT);
  digitalWrite(LED_DIR,  LOW);
  digitalWrite(LED_SOL,  LOW);
  digitalWrite(LED_SEP,  LOW);
  digitalWrite(LED_ESP,  LOW);
  digitalWrite(LED_LINE, LOW);

  gSol.begin(LED_SOL);
  gMov.begin(LED_DIR, LED_SEP, LED_ESP, LED_LINE);

  gParams.t_golpe_ms = T_GOLPE_MS;
  gParams.retornoAlMargen = true;
  gPrn.begin(&gMov, &gSol, gParams);

  // BANNER de arranque (ahora sí aparece)
  Serial.println();
  Serial.println(F("============================================"));
  Serial.println(F("  ESP32 Braille - DEMO eventos + VISTA ZIGZAG"));
  Serial.println(F("============================================"));
  Serial.println(F("  LED_DIR  (GPIO33): ON=RTL, OFF=LTR"));
  Serial.println(F("  LED_SOL  (GPIO25): Golpe"));
  Serial.println(F("  LED_SEP  (GPIO26): Separación corta"));
  Serial.println(F("  LED_ESP  (GPIO27): Espacio mediano"));
  Serial.println(F("  LED_LINE (GPIO14): Salto de línea"));
  Serial.println();
  Serial.println(F(" Protocolo:"));
  Serial.println(F("   CELLS M N"));
  Serial.println(F("   <M lineas de texto>"));
  Serial.println(F("   END"));
  Serial.println();
  Serial.println(F(" Ejemplo:"));
  Serial.println(F("   CELLS 1 10"));
  Serial.println(F("   HOLA MUNDO"));
  Serial.println(F("   END"));
  Serial.println("============================================\n");
  Serial.println(F(" Listo. Ingresa la cabecera 'CELLS M N'..."));
}

// ====== Loop ======
bool leerCabecera(size_t& M, size_t& N); // fwd (ya definida arriba)

void loop() {
  if (Serial.available() == 0) { delay(10); return; }

  size_t M=0, N=0;
  if (!leerCabecera(M, N)) {
    Serial.println(F("[Error] Cabecera inválida. Usa 'CELLS M N'."));
    unsigned long t0 = millis();
    while (millis() - t0 < 500) { if (Serial.available()) Serial.read(); }
    Serial.println(F("Ingresa de nuevo 'CELLS M N'..."));
    return;
  }

  Serial.print(F("OK. M=")); Serial.print(M);
  Serial.print(F(", N=")); Serial.print(N);
  Serial.println(F(". Escribe las líneas y finaliza con 'END'."));

  // Reservar matriz MxN
  char* matriz_texto = (char*) malloc(M*N);
  if (!matriz_texto) {
    Serial.println(F("[Error] Sin memoria para matriz_texto."));
    return;
  }

  size_t lineasLeidas = 0;
  while (true) {
    while (Serial.available() == 0) { delay(5); }
    String line = rtrim(Serial.readStringUntil('\n'));
    if (line == "END") break;

    if (lineasLeidas < M) {
      char* row = matriz_texto + (lineasLeidas * N);
      for (size_t j=0; j<N; ++j) row[j] = ' ';
      size_t len = (size_t) line.length();
      if (len > N) len = N;
      for (size_t j=0; j<len; ++j) row[j] = line.charAt(j);
      lineasLeidas++;
    }
  }
  while (lineasLeidas < M) {
    char* row = matriz_texto + (lineasLeidas * N);
    for (size_t j=0; j<N; ++j) row[j] = ' ';
    lineasLeidas++;
  }

  // Traducir a matriz de puntos (3*M x 2*N)
  size_t rows = M * 3;
  size_t cols = N * 2;
  uint8_t* braille = (uint8_t*) malloc(rows * cols);
  if (!braille) {
    Serial.println(F("[Error] Sin memoria para matriz_braille_bits."));
    free(matriz_texto);
    return;
  }

  traducirBraille(M, N, matriz_texto, braille);

  // --- Debug: ANTES (base) ---
  imprimirMatrizBase(braille, rows, cols);

  // --- Debug: DESPUÉS (vista zigzag como se recorrerá) ---
  imprimirVistaZigzag(braille, rows, cols);

  // --- Impresión por eventos (usa mismo criterio: fila 0 RTL) ---
  gPrn.printMatrix(braille, rows, cols);

  Serial.println(F("Listo. Puedes enviar otra cabecera 'CELLS M N'.\n"));

  free(braille);
  free(matriz_texto);
}
