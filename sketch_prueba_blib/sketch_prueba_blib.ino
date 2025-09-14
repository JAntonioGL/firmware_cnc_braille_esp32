#include <Arduino.h>
#include <vector>
#include <string>
#include "braille_lib.h"

// Variables para almacenar las dimensiones y el mensaje del usuario
int brailleMaxRows;
int brailleMaxCols;
std::string userMessage;

// Enum para controlar el estado del programa
enum State {
  WAITING_FOR_ROWS,
  WAITING_FOR_COLS,
  WAITING_FOR_TEXT,
  PROCESSING_TEXT
};

// Variable para rastrear el estado actual
State currentState = WAITING_FOR_ROWS;

void setup() {
    Serial.begin(115200);
    // Agregamos un pequeño retraso para asegurar que la conexión serial esté lista
    delay(1000); 

    Serial.println("Listo para recibir la configuracion.");
    Serial.println("Ingresa el numero maximo de FILAS y presiona Enter:");
}

void loop() {
    // Comprueba si hay datos disponibles en el puerto serial
    if (Serial.available() > 0) {
        String inputString = Serial.readStringUntil('\n');
        std::string cppInputString = inputString.c_str();

        // Limpia la entrada para evitar espacios en blanco
        size_t firstChar = cppInputString.find_first_not_of(" \t\r\n");
        if (std::string::npos == firstChar) {
            return; // No hace nada si la entrada está vacía
        }
        
        switch (currentState) {
            case WAITING_FOR_ROWS:
                brailleMaxRows = std::stoi(cppInputString);
                Serial.println("Numero de filas recibido.");
                Serial.println("Ingresa el numero maximo de COLUMNAS y presiona Enter:");
                currentState = WAITING_FOR_COLS;
                break;
                
            case WAITING_FOR_COLS:
                brailleMaxCols = std::stoi(cppInputString);
                Serial.println("Numero de columnas recibido.");
                Serial.println("Ahora, ingresa el MENSAJE y presiona Enter:");
                currentState = WAITING_FOR_TEXT;
                break;
                
            case WAITING_FOR_TEXT:
                userMessage = cppInputString;
                currentState = PROCESSING_TEXT;
                break;
                
            case PROCESSING_TEXT:
                // Las variables ya fueron recibidas, ahora procesamos la solicitud
                // Crea la matriz con las dimensiones especificadas
                std::vector<std::vector<BraillePointState>> brailleSheet(brailleMaxRows * 3, std::vector<BraillePointState>(brailleMaxCols * 2));

                // Llama a la funcion de la biblioteca para llenar la matriz
                translateToBrailleMatrix(userMessage, brailleSheet);
                
                // Imprime UNICAMENTE la matriz braille en el monitor serial
                for (int i = 0; i < brailleMaxRows * 3; ++i) {
                    for (int j = 0; j < brailleMaxCols * 2; ++j) {
                        Serial.print(brailleSheet[i][j] == BRAILLE_HIGH ? 'o' : '.');
                        if ((j + 1) % 2 == 0) {
                            Serial.print(' ');
                        }
                    }
                    Serial.println();
                    if ((i + 1) % 3 == 0) {
                        Serial.println();
                    }
                }
                
                // Resetea el estado para un nuevo ciclo
                Serial.println("---");
                Serial.println("Procesado. Ingresa el numero maximo de FILAS para empezar de nuevo:");
                currentState = WAITING_FOR_ROWS;
                break;
        }
    }
}