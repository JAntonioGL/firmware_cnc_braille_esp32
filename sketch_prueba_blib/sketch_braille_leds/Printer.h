// Printer.h
#pragma once
#include <Arduino.h>
#include "Movement.h"
#include "Solenoid.h"

struct PrintParams {
  uint16_t t_hold_ms = 15;    // tiempo de golpe
  uint16_t t_rel_ms  = 5;     // relajación
  bool retornarAlMargen = true;
};

class Printer {
public:
  void begin(Movement* mov, Solenoid* sol, PrintParams params) {
    _mov = mov; _sol = sol; _p = params;
  }

  // braille: matriz de (rows x cols), con rows = 3*M y cols = 2*N
  // Recorrido zigzag por filas: par normal, impar espejo
  void printMatrix(const uint8_t* braille, size_t rows, size_t cols) {
    if (!_mov || !_sol) return;

    for (size_t r=0; r<rows; ++r) {
      bool espejo = (r % 2) == 1;     // filas impares en espejo (zigzag)
      _mov->setZigzagDirection(espejo);

      // Recorremos columnas según sentido lógico
      if (!espejo) {
        // izq → der
        for (size_t c=0; c<cols; ++c) {
          processDot(braille[r*cols + c]);
        }
      } else {
        // der → izq (leer matriz de der a izq)
        for (int c=int(cols)-1; c>=0; --c) {
          processDot(braille[r*cols + c]);
        }
      }

      // Fin de fila: salto de línea y retorno X si aplica
      uint32_t retorno = _mov->filaDistAcumulada();
      if (_mov->moveSaltoLinea(_p.retornarAlMargen, retorno) != MoveResult::OK) {
        // TODO: manejar error/limite
        break;
      }
    }
  }

private:
  Movement* _mov = nullptr;
  Solenoid* _sol = nullptr;
  PrintParams _p;

  void processDot(uint8_t bit) {
    // Golpe si hay 1
    if (bit) _sol->pulse(_p.t_hold_ms, _p.t_rel_ms);

    // Avance “corto” entre puntos del mismo símbolo
    _mov->moveSepCorto();

    // NOTA: el “espacio mediano” entre símbolos (celdas) lo decides
    // externamente si recorres por celdas; en este recorrido por
    // puntos crudos, puedes aplicar “mediano” cada 2 columnas si lo deseas.
    // Ejemplo sencillo: cada 2 puntos avanzados, añadir un mediano:
    static uint16_t puntoCount = 0;
    puntoCount++;
    if (puntoCount % 2 == 0) {
      _mov->moveEspMediano();
    }
  }
};
