// Translator.h
#pragma once
#include <Arduino.h>

void traducirBraille(size_t M, size_t N, const char* matriz_texto, uint8_t* out_bits);
