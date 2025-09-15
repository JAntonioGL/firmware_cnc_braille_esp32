// Braille.h
#pragma once
#include <Arduino.h>

struct Glyph {
  char ch;
  uint8_t dots[3][2];
};

extern const Glyph BRAILLE_MAP[];
extern const size_t BRAILLE_MAP_LEN;

bool brailleLookup(char ch, uint8_t out[3][2]);
