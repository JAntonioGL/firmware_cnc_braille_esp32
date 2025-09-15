// Braille.cpp
// Biblioteca de glifos Braille 6 puntos (formato 3x2) para español.
// Convención de puntos:
// (fila,col): (0,0)=1, (1,0)=2, (2,0)=3, (0,1)=4, (1,1)=5, (2,1)=6

#include "Braille.h"

const Glyph BRAILLE_MAP[] = {
  // A–J
  {'A', {{1,0},{0,0},{0,0}}},
  {'B', {{1,0},{1,0},{0,0}}},
  {'C', {{1,1},{0,0},{0,0}}},
  {'D', {{1,1},{0,1},{0,0}}},
  {'E', {{1,0},{0,1},{0,0}}},
  {'F', {{1,1},{1,0},{0,0}}},
  {'G', {{1,1},{1,1},{0,0}}},
  {'H', {{1,0},{1,1},{0,0}}},
  {'I', {{0,1},{1,0},{0,0}}},
  {'J', {{0,1},{1,1},{0,0}}},

  // K–T
  {'K', {{1,0},{0,0},{1,0}}},
  {'L', {{1,0},{1,0},{1,0}}},
  {'M', {{1,1},{0,0},{1,0}}},
  {'N', {{1,1},{0,1},{1,0}}},
  {'O', {{1,0},{0,1},{1,0}}},
  {'P', {{1,1},{1,0},{1,0}}},
  {'Q', {{1,1},{1,1},{1,0}}},
  {'R', {{1,0},{1,1},{1,0}}},
  {'S', {{0,1},{1,0},{1,0}}},
  {'T', {{0,1},{1,1},{1,0}}},

  // U–Z
  {'U', {{1,0},{0,0},{1,1}}},
  {'V', {{1,0},{1,0},{1,1}}},
  {'W', {{0,1},{1,1},{0,1}}}, // especial
  {'X', {{1,1},{0,0},{1,1}}},
  {'Y', {{1,1},{0,1},{1,1}}},
  {'Z', {{1,0},{0,1},{1,1}}},

  // Espacio
  {' ', {{0,0},{0,0},{0,0}}},
};

const size_t BRAILLE_MAP_LEN = sizeof(BRAILLE_MAP) / sizeof(BRAILLE_MAP[0]);

bool brailleLookup(char ch, uint8_t out[3][2]) {
  // Normaliza a mayúsculas si es letra ASCII
  if (ch >= 'a' && ch <= 'z') ch = char(ch - 'a' + 'A');

  // Busca en la tabla
  for (size_t i = 0; i < BRAILLE_MAP_LEN; ++i) {
    if (BRAILLE_MAP[i].ch == ch) {
      for (int r = 0; r < 3; ++r) {
        out[r][0] = BRAILLE_MAP[i].dots[r][0];
        out[r][1] = BRAILLE_MAP[i].dots[r][1];
      }
      return true;
    }
  }

  // Desconocido → tratar como espacio
  for (int r = 0; r < 3; ++r) {
    out[r][0] = 0;
    out[r][1] = 0;
  }
  return false;
}
