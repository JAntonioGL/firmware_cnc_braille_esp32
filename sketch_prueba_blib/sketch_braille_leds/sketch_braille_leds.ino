/*
 * MAIN: Sistema Braille Modular - Simulación LED
 */

#include "HardwareSim.h"
#include "Coordinador.h"
#include "Translator.h" // Tu traductor existente (asegúrate de incluirlo)
#include "Braille.h"    // Tu librería de glifos existente

// Instancias Globales
HardwareSim hw;
Coordinador jefe(&hw);

void setup() {
  Serial.begin(115200);
  hw.begin(); // Configura los pines de los LEDs
  
  Serial.println("SISTEMA BRAILLE INICIADO (MODO SIMULACION LED)");
  Serial.println("Envia cabecera 'CELLS M N' y luego el texto...");
}

// Variables para manejar la entrada (simplificado de tu código anterior)
bool esperandoTexto = false;
size_t M_global = 0;
size_t N_global = 0;

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() == 0) return;

    // 1. Detectar Cabecera
    if (input.startsWith("CELLS")) {
      // Parsear M y N (Simificado para el ejemplo)
      int espacio1 = input.indexOf(' ');
      int espacio2 = input.lastIndexOf(' ');
      M_global = input.substring(espacio1, espacio2).toInt();
      N_global = input.substring(espacio2).toInt();
      
      Serial.print("CONFIGURACION RECIBIDA: M="); Serial.print(M_global);
      Serial.print(" N="); Serial.println(N_global);
      Serial.println("Esperando texto...");
      esperandoTexto = true;
      return;
    }

    // 2. Procesar Texto si ya tenemos configuración
    if (esperandoTexto) {
       Serial.print("TEXTO RECIBIDO: ");
       Serial.println(input);
       
       // A. Crear buffers
       size_t rows = M_global * 3;
       size_t cols = N_global * 2;
       uint8_t* braille_bits = (uint8_t*) malloc(rows * cols);
       
       // B. Traducir (Usando tu función existente)
       // Nota: Adapta esto si tu función recibirMatrizTexto es distinta, 
       // pero la lógica es: Texto -> Matriz Bits Lineal.
       // Para el ejemplo, asumo que 'input' es todo el texto de una.
       traducirBraille(M_global, N_global, input.c_str(), braille_bits); // Tu función
       
       // C. Mostrar Matriz Lineal (Raw)
       Serial.println("\n[DEBUG] Matriz Lineal Traducida (Memoria):");
       for(size_t i=0; i<rows*cols; i++) {
          Serial.print(braille_bits[i]);
          if((i+1)%cols == 0) Serial.println();
       }

       // D. Mostrar Matriz ZigZag (Visualización lógica)
       jefe.debugImprimirZigZag(braille_bits, rows, cols);

       // E. EJECUTAR SECUENCIA LEDS
       // Aquí ocurre la magia visual con los LEDs
       jefe.ejecutarSecuencia(braille_bits, rows, cols);

       // Limpieza
       free(braille_bits);
       esperandoTexto = false;
       Serial.println("Listo. Esperando nueva orden 'CELLS'...");
    }
  }
}