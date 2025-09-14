#include "braille_lib.h"
#include <map>
#include <cctype>

// Mapeo global de caracteres a sus celdas braille de 3x2
const std::map<char, BrailleCell> brailleMap = {
    // Abecedario
    {'a', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'b', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'c', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'d', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'e', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'f', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'g', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'h', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'i', {{{BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'j', {{{BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'k', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'l', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'m', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'n', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'o', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'p', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'q', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'r', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'s', {{{BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'t', {{{BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'u', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    {'v', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    {'w', {{{BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_HIGH}}}},
    {'x', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    {'y', {{{BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    {'z', {{{BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    // Símbolos de control
    {'\n', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    // Símbolos de puntuación
    {' ', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {',', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {';', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {':', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    {'.', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_LOW}}}},
    {'!', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    {'?', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_HIGH}}}},
    {'-', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    {'"', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
    {'\'',{{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_HIGH}, {BRAILLE_LOW, BRAILLE_LOW}}}},
    // Símbolos de prefijo
    {'_', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_LOW, BRAILLE_HIGH}}}},
    {'#', {{{BRAILLE_LOW, BRAILLE_LOW}, {BRAILLE_HIGH, BRAILLE_HIGH}, {BRAILLE_HIGH, BRAILLE_HIGH}}}},
};

void translateToBrailleMatrix(const std::string& text, 
                              std::vector<std::vector<BraillePointState>>& brailleGrid) {
    
    const int rows = brailleGrid.size();
    if (rows == 0) return;
    const int cols = brailleGrid[0].size();
    if (cols == 0) return;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            brailleGrid[i][j] = BRAILLE_LOW;
        }
    }

    int currentBrailleRow = 0;
    int currentBrailleCol = 0;

    for (char c : text) {
        if (c == '\n') {
            currentBrailleRow++;
            currentBrailleCol = 0;
            continue;
        }

        char lower_c = tolower(c);

        if (isupper(c)) {
            if (currentBrailleCol * 2 + 1 < cols) {
                const auto& prefixCell = brailleMap.at('_');
                for (int i = 0; i < 3; ++i) {
                    brailleGrid[currentBrailleRow * 3 + i][currentBrailleCol * 2]     = prefixCell.points[i][0];
                    brailleGrid[currentBrailleRow * 3 + i][currentBrailleCol * 2 + 1] = prefixCell.points[i][1];
                }
                currentBrailleCol++;
            }
        }
        
        if (isdigit(c)) {
             if (currentBrailleCol * 2 + 1 < cols) {
                const auto& prefixCell = brailleMap.at('#');
                for (int i = 0; i < 3; ++i) {
                    brailleGrid[currentBrailleRow * 3 + i][currentBrailleCol * 2]     = prefixCell.points[i][0];
                    brailleGrid[currentBrailleRow * 3 + i][currentBrailleCol * 2 + 1] = prefixCell.points[i][1];
                }
                currentBrailleCol++;
            }
        }

        if (currentBrailleCol * 2 + 1 < cols && currentBrailleRow * 3 + 2 < rows) {
            if (brailleMap.count(lower_c)) {
                const auto& brailleCell = brailleMap.at(lower_c);
                
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 2; ++j) {
                        brailleGrid[currentBrailleRow * 3 + i][currentBrailleCol * 2 + j] = brailleCell.points[i][j];
                    }
                }
                currentBrailleCol++;
            }
        } else {
            break;
        }
    }
}