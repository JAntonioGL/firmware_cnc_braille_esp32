#ifndef BRAILLE_LIB_H
#define BRAILLE_LIB_H

#include <string>
#include <vector>

// Define el estado de un punto braille con nombres que no entren en conflicto con Arduino.
enum BraillePointState {
    BRAILLE_LOW = 0,
    BRAILLE_HIGH = 1
};

// Usa un struct para una mejor compatibilidad del compilador
struct BrailleCell {
    BraillePointState points[3][2];
};

// Prototipo de la función que traduce el texto a la matriz braille
void translateToBrailleMatrix(const std::string& text, 
                              std::vector<std::vector<BraillePointState>>& brailleGrid);

#endif // BRAILLE_LIB_H