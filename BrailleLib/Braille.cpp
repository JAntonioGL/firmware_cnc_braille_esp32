#include "Braille.h"
#include <string.h>

// ===== Helper: decodificar UTF-8 a codepoints =====
static bool utf8_next(const char* s, size_t len, size_t& i, uint32_t& cp) {
  if (i >= len) return false;
  uint8_t c = (uint8_t)s[i++];
  if (c < 0x80) { cp = c; return true; }
  if ((c >> 5) == 0x6 && i < len) {
    uint8_t c1 = (uint8_t)s[i++];
    if ((c1 & 0xC0) != 0x80) return false;
    cp = ((c & 0x1F) << 6) | (c1 & 0x3F);
    return true;
  }
  if ((c >> 4) == 0xE && i + 1 < len) {
    uint8_t c1 = (uint8_t)s[i++], c2 = (uint8_t)s[i++];
    if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80) return false;
    cp = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
    return true;
  }
  cp = '?'; return true; // simplificación
}

// ===== Mapeo Braille =====
// Bits: b0:p1, b1:p2, b2:p3, b3:p4, b4:p5, b5:p6

static uint8_t braille_for_codepoint(uint32_t cp) {
  // espacio
  if (cp == ' ') return 0;

  // letras minúsculas
  if (cp >= 'A' && cp <= 'Z') cp = (uint32_t)('a' + (cp - 'A'));
  switch (cp) {
    case 'a': return 0b000001;       // 1
    case 'b': return 0b000011;       // 1,2
    case 'c': return 0b000101;       // 1,4
    case 'd': return 0b001101;       // 1,4,5
    case 'e': return 0b001001;       // 1,5
    case 'f': return 0b000111;       // 1,2,4
    case 'g': return 0b001111;       // 1,2,4,5
    case 'h': return 0b001011;       // 1,2,5
    case 'i': return 0b000110;       // 2,4
    case 'j': return 0b001110;       // 2,4,5
    case 'k': return 0b000101 | 0b000100; // 1,3
    case 'l': return 0b000111 | 0b000100; // 1,2,3
    case 'm': return 0b000101 | 0b000100; // 1,3,4
    case 'n': return 0b001101 | 0b000100; // 1,3,4,5
    case 'o': return 0b001001 | 0b000100; // 1,3,5
    case 'p': return 0b000111 | 0b000100; // 1,2,3,4
    case 'q': return 0b001111 | 0b000100; // 1,2,3,4,5
    case 'r': return 0b001011 | 0b000100; // 1,2,3,5
    case 's': return 0b000110 | 0b000100; // 2,3,4
    case 't': return 0b001110 | 0b000100; // 2,3,4,5
    case 'u': return 0b000101 | 0b000100 | 0b100000; // 1,3,6
    case 'v': return 0b000111 | 0b000100 | 0b100000; // 1,2,3,6
    case 'w': return 0b001110 | 0b100000;            // 2,4,5,6
    case 'x': return 0b000101 | 0b000100 | 0b100000; // 1,3,4,6
    case 'y': return 0b001101 | 0b000100 | 0b100000; // 1,3,4,5,6
    case 'z': return 0b001001 | 0b000100 | 0b100000; // 1,3,5,6
  }

  // fallback
  return 0;
}

// Escribir una celda 3x2 espejada (RTL)
static void blit_cell(uint8_t mask, uint8_t* mat, size_t M, size_t N,
                      size_t rowBase, size_t colBase) {
  if (rowBase + 2 >= M || colBase + 1 >= N) return;

  uint8_t p1 = (mask >> 0) & 1;
  uint8_t p2 = (mask >> 1) & 1;
  uint8_t p3 = (mask >> 2) & 1;
  uint8_t p4 = (mask >> 3) & 1;
  uint8_t p5 = (mask >> 4) & 1;
  uint8_t p6 = (mask >> 5) & 1;

  // espejo: [p4 p1], [p5 p2], [p6 p3]
  mat[(rowBase+0)*N + (colBase+0)] = p4;
  mat[(rowBase+0)*N + (colBase+1)] = p1;
  mat[(rowBase+1)*N + (colBase+0)] = p5;
  mat[(rowBase+1)*N + (colBase+1)] = p2;
  mat[(rowBase+2)*N + (colBase+0)] = p6;
  mat[(rowBase+2)*N + (colBase+1)] = p3;
}

namespace Braille {

void ComputeTextCellDims(const char* text_utf8,
                         size_t* out_lines,
                         size_t* out_max_cols) {
  if (out_lines) *out_lines = 0;
  if (out_max_cols) *out_max_cols = 0;
  if (!text_utf8) return;

  size_t len = strlen(text_utf8), i = 0;
  size_t lines = 1, cur_cols = 0, max_cols = 0;

  while (i < len) {
    uint32_t cp=0; size_t prev=i;
    if (!utf8_next(text_utf8,len,i,cp)) { cp=' '; i=prev+1; }
    if (cp=='\n') { if(cur_cols>max_cols)max_cols=cur_cols; cur_cols=0; lines++; }
    else cur_cols++;
  }
  if (cur_cols>max_cols) max_cols=cur_cols;

  if (out_lines) *out_lines = lines;
  if (out_max_cols) *out_max_cols = max_cols;
}

void TextToMatrix(const char* text_utf8,
                  size_t M, size_t N,
                  uint8_t* outMatrix) {
  if (!outMatrix) return;
  memset(outMatrix,0,M*N);
  if (!text_utf8) return;

  size_t len=strlen(text_utf8), i=0;
  size_t line=0, colChar=0;

  while(i<len) {
    if (line*3+2>=M) break;
    uint32_t cp=0; size_t prev=i;
    if(!utf8_next(text_utf8,len,i,cp)){ cp=' '; i=prev+1; }

    if(cp=='\n'){ line++; colChar=0; continue; }

    size_t colBase=2*colChar;
    if(colBase+1<N){
      uint8_t mask=braille_for_codepoint(cp);
      blit_cell(mask,outMatrix,M,N,3*line,colBase);
      colChar++;
    }
  }
}

} // namespace Braille
