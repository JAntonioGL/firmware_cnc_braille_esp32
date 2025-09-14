#pragma once
#include <stddef.h>
#include <stdint.h>

namespace Braille {

/// Traduce un texto en UTF-8 a una matriz MxN de puntos Braille (0/1).
/// - Cada carácter ocupa una celda 3x2 (3 filas, 2 columnas).
/// - Cada salto de línea '\n' baja 3 filas.
/// - La matriz resultante ya está en "modo espejo" por celda para barrido RTL.
/// - El resto de la matriz se rellena con ceros.
/// - Caracteres fuera del rango se convierten en espacio.
///
/// Parámetros:
/// - text_utf8: cadena UTF-8 a traducir.
/// - M: número de filas (en puntos).
/// - N: número de columnas (en puntos).
/// - outMatrix: buffer de tamaño M*N (row-major).
void TextToMatrix(const char* text_utf8,
                  size_t M, size_t N,
                  uint8_t* outMatrix);

/// Devuelve cuántas líneas (separadas por '\n') y la longitud máxima de línea
/// en caracteres (celdas) hay en el texto.
void ComputeTextCellDims(const char* text_utf8,
                         size_t* out_lines,
                         size_t* out_max_cols);

} // namespace Braille
