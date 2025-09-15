// Movement.h
#pragma once
#include <Arduino.h>

enum class MoveResult : uint8_t { OK, LIMIT_HIT, ERROR };

class Movement {
public:
  void begin(uint8_t pinStepX, uint8_t pinDirX, uint8_t pinStepY, uint8_t pinDirY) {
    _pinStepX = pinStepX; _pinDirX = pinDirX;
    _pinStepY = pinStepY; _pinDirY = pinDirY;
    pinMode(_pinStepX, OUTPUT); pinMode(_pinDirX, OUTPUT);
    pinMode(_pinStepY, OUTPUT); pinMode(_pinDirY, OUTPUT);
    // TODO: homing real
  }

  // Configura “pitchs”: pasos para corto/mediano y salto de línea
  void setPitches(uint16_t x_corto_steps, uint16_t x_mediano_steps, uint16_t y_line_steps) {
    _xCorto = x_corto_steps; _xMediano = x_mediano_steps; _yLine = y_line_steps;
  }

  // Sentido actual por fila zigzag: true=der→izq, false=izq→der
  void setZigzagDirection(bool rightToLeft) { _rtl = rightToLeft; }
  bool zigzagDirection() const { return _rtl; }

  // “Separación” corta entre puntos del mismo símbolo
  MoveResult moveSepCorto()  { return moveX(_xCorto); }

  // “Espacio” mediano entre símbolos (celdas)
  MoveResult moveEspMediano(){ return moveX(_xMediano); }

  // Salto de línea (avanza Y y opcionalmente regresa X al margen)
  MoveResult moveSaltoLinea(bool regresarAlMargen, uint32_t xRetornoSteps) {
    // Avanza Y
    if (moveY(_yLine) != MoveResult::OK) return MoveResult::ERROR;
    // Retorno X al margen (para impresión en boustrofedón)
    if (regresarAlMargen) {
      // Vuelve todo el ancho recorrido en la fila anterior
      return moveX(xRetornoSteps, /*forceLeft=*/ !_rtl);
    }
    return MoveResult::OK;
  }

  // (Opcional) obtén la distancia X recorrida en la fila actual (para saber retorno)
  uint32_t filaDistAcumulada() const { return _filaDist; }
  void resetFilaDist() { _filaDist = 0; }

private:
  uint8_t _pinStepX=255,_pinDirX=255,_pinStepY=255,_pinDirY=255;
  uint16_t _xCorto=1, _xMediano=2, _yLine=10;
  bool _rtl=false; // false: izq→der; true: der→izq
  uint32_t _filaDist=0;

  MoveResult moveX(uint32_t steps, bool forceLeft = false) {
    // Dirección según zigzag o forzada
    bool dirLeft = forceLeft ? true : _rtl; // si _rtl=true, vamos a la izquierda
    digitalWrite(_pinDirX, dirLeft ? HIGH : LOW);
    for (uint32_t i=0;i<steps;i++) {
      // TODO: aquí metes aceleración, PID o verificación encoder
      digitalWrite(_pinStepX, HIGH); delayMicroseconds(300);
      digitalWrite(_pinStepX, LOW ); delayMicroseconds(300);
      _filaDist++;
    }
    return MoveResult::OK;
  }

  MoveResult moveY(uint32_t steps) {
    // Suponemos Y siempre “positivo” hacia abajo
    digitalWrite(_pinDirY, LOW);
    for (uint32_t i=0;i<steps;i++) {
      digitalWrite(_pinStepY, HIGH); delayMicroseconds(300);
      digitalWrite(_pinStepY, LOW ); delayMicroseconds(300);
    }
    resetFilaDist();
    return MoveResult::OK;
  }
};
