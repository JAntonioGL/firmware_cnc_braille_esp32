// Translator.cpp
#include "Translator.h"
#include "Braille.h"

void traducirBraille(size_t M, size_t N, const char* matriz_texto, uint8_t* out_bits) {
  auto OUT = [&](size_t r, size_t c) -> uint8_t& {
    return out_bits[r*(N*2) + c];
  };
  for (size_t i=0; i<M; i++) {
    for (size_t j=0; j<N; j++) {
      char ch = matriz_texto[i*N + j];
      uint8_t g[3][2];
      brailleLookup(ch, g);
      size_t baseR = i*3;
      size_t baseC = j*2;
      for (int r=0; r<3; r++) {
        OUT(baseR+r, baseC+0) = g[r][0];
        OUT(baseR+r, baseC+1) = g[r][1];
      }
    }
  }
}
