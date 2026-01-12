#ifndef COORDINADOR_H
#define COORDINADOR_H

#include <Arduino.h>
#include "HardwareSim.h"

class Coordinador {
public:
    Coordinador(HardwareSim* hw);

    // Función 1: Solo para mostrar en Serial la matriz visualmente (Debug)
    void debugImprimirZigZag(const uint8_t* matriz, size_t filas, size_t cols);

    // Función 2: Ejecuta la secuencia de luces/movimiento
    void ejecutarSecuencia(const uint8_t* matriz, size_t filas, size_t cols);

private:
    HardwareSim* _hw;
    // Helper para obtener el dato corregido por ZigZag
    bool leerBitZigZag(const uint8_t* matriz, size_t f, size_t c, size_t ancho);
};

#endif