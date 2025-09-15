#include <Arduino.h>
#include "Braille.h"
#include "Translator.h"


// ==== pega aquí la Biblioteca Braille y las funciones brailleLookup/traducirBraille de arriba ====

static const unsigned long BAUD = 115200;

// Utilidad sencilla para recortar fin de línea
String rtrim(const String& s) {
  int end = s.length();
  while (end > 0 && (s[end-1] == '\r' || s[end-1] == '\n')) end--;
  return s.substring(0, end);
}

void setup() {
  Serial.begin(BAUD);
  while (!Serial) {}
  Serial.println(F("Listo. Protocolo:"));
  Serial.println(F("  CELLS M N"));
  Serial.println(F("  <M lineas de texto (con o sin \\n)>"));
  Serial.println(F("  END"));
  Serial.println(F("Ejemplo:"));
  Serial.println(F("  CELLS 1 10"));
  Serial.println(F("  HOLA MUNDO"));
  Serial.println(F("  END"));
}

bool leerCabecera(size_t& M, size_t& N) {
  // Espera una línea que comience con "CELLS "
  while (Serial.available() == 0) { delay(5); }
  String header = rtrim(Serial.readStringUntil('\n'));
  header.trim();
  if (!header.startsWith("CELLS")) return false;

  // Parsear
  int i1 = header.indexOf(' ');
  int i2 = header.indexOf(' ', i1+1);
  if (i1 < 0 || i2 < 0) return false;
  String sM = header.substring(i1+1, i2);
  String sN = header.substring(i2+1);
  M = (size_t) sM.toInt();
  N = (size_t) sN.toInt();
  return (M > 0 && N > 0);
}

void loop() {
  if (Serial.available() == 0) { delay(10); return; }

  size_t M=0, N=0;
  if (!leerCabecera(M, N)) {
    Serial.println(F("Error: cabecera invalida. Usa 'CELLS M N'"));
    // Leer hasta END para limpiar
    while (Serial.available()) { Serial.read(); }
    return;
  }

  // Reservar matriz_texto MxN (linealizada)
  char* matriz_texto = (char*) malloc(M*N);
  if (!matriz_texto) {
    Serial.println(F("Error: sin memoria para matriz_texto."));
    return;
  }

  // Leer lineas hasta END (se esperan al menos M lineas de texto)
  size_t lineasLeidas = 0;
  while (true) {
    while (Serial.available() == 0) { delay(5); }
    String line = rtrim(Serial.readStringUntil('\n'));
    if (line == "END") break;

    // Si AÚN no completamos M filas, copiamos la línea a la fila actual
    if (lineasLeidas < M) {
      // Rellenar con espacios
      char* row = matriz_texto + (lineasLeidas * N);
      for (size_t j=0; j<N; ++j) row[j] = ' ';
      // Copiar hasta N chars de la linea
      size_t len = (size_t) line.length();
      if (len > N) len = N;
      for (size_t j=0; j<len; ++j) row[j] = line.charAt(j);
      lineasLeidas++;
    }
    // Si llegan más líneas que M, simplemente las ignoramos (la biblioteca NO trunca internamente).
  }

  // Si faltaron líneas, rellenar filas restantes con espacios
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
    Serial.println(F("Error: sin memoria para matriz_braille_bits."));
    free(matriz_texto);
    return;
  }
  traducirBraille(M, N, matriz_texto, braille);

  // Imprimir matriz de puntos
  Serial.println(F("== MATRIZ BRAILLE (3*M filas x 2*N cols) =="));
  for (size_t r=0; r<rows; ++r) {
    String line;
    line.reserve(cols + N); // espacio extra para separadores
    for (size_t c=0; c<cols; ++c) {
      line += (braille[r*cols + c] ? '1' : '0');
      // separador visual cada 2 columnas (limites de celda)
      if ((c % 2)==1 && c+1 < cols) line += ' ';
    }
    Serial.println(line);
  }
  Serial.println(F("== FIN =="));

  free(braille);
  free(matriz_texto);
}
