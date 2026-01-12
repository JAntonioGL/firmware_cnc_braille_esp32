#include "Coordinador.h"

Coordinador::Coordinador(HardwareSim* hw) {
    _hw = hw;
}

// Lógica mágica del ZigZag: Transforma coordenadas (f, c) en índice real del array
bool Coordinador::leerBitZigZag(const uint8_t* matriz, size_t f, size_t c, size_t ancho) {
    bool esFilaPar = (f % 2 == 0); // Ida
    size_t indiceReal;

    if (esFilaPar) {
        // Lectura normal: Izquierda a Derecha
        indiceReal = (f * ancho) + c;
    } else {
        // Lectura espejo: Derecha a Izquierda
        // (ancho - 1 - c) invierte la columna
        indiceReal = (f * ancho) + (ancho - 1 - c);
    }
    return (matriz[indiceReal] == 1);
}

void Coordinador::debugImprimirZigZag(const uint8_t* matriz, size_t filas, size_t cols) {
    Serial.println("\n--- VISTA PREVIA MATRIZ ZIGZAG (Como se imprimirá) ---");
    for (size_t f = 0; f < filas; f++) {
        bool esIda = (f % 2 == 0);
        Serial.print(f); 
        Serial.print(esIda ? " (IDA)   : " : " (VUELTA): ");
        
        for (size_t c = 0; c < cols; c++) {
            // Usamos la misma lógica que usará el motor para mostrarlo en pantalla
            Serial.print(leerBitZigZag(matriz, f, c, cols) ? "O " : ". ");
        }
        Serial.println();
    }
    Serial.println("------------------------------------------------------\n");
}

void Coordinador::ejecutarSecuencia(const uint8_t* matriz, size_t filas, size_t cols) {
    Serial.println("INICIANDO SECUENCIA DE IMPRESION SIMULADA...");
    
    for (size_t f = 0; f < filas; f++) {
        // 1. Configurar Dirección Física
        bool esIda = (f % 2 == 0);
        _hw->setDireccionX(esIda);

        // 2. Recorrer Columnas (Puntos)
        for (size_t c = 0; c < cols; c++) {
            
            // A. ¿GOLPEAR? Consultamos lógica ZigZag
            if (leerBitZigZag(matriz, f, c, cols)) {
                _hw->golpear(); // LED ROJO
            } else {
                // Serial.print("."); // Espacio vacío
            }

            // B. ¿MOVER X? (Si no es la última columna)
            if (c < cols - 1) {
                // Determinar tipo de salto X
                // Columna par (0, 2...) -> Salto intra-letra (Corto)
                // Columna impar (1, 3...) -> Salto entre-letras (Largo)
                if (c % 2 == 0) {
                    _hw->moverX_Corto(); // LED VERDE
                } else {
                    _hw->moverX_Largo(); // LED AZUL
                }
            }
        }

        // C. ¿MOVER Y? (Si no es la última fila)
        if (f < filas - 1) {
            // Un caracter Braille tiene 3 filas de alto (0,1,2).
            // Si terminamos fila 2, 5, 8... es salto de línea de texto.
            if ((f + 1) % 3 == 0) {
                _hw->moverY_Largo(); // LED NARANJA (Salto de renglón)
            } else {
                _hw->moverY_Corto(); // LED AMARILLO (Salto de punto)
            }
        }
    }
    Serial.println("FIN DE IMPRESION.\n");
}