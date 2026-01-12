#ifndef HARDWARE_SIM_H
#define HARDWARE_SIM_H
#include <Arduino.h>

// Definición de Pines para la simulación
#define PIN_LED_SOLENOIDE 25 // Rojo
#define PIN_LED_X1        26 // Verde (Paso corto intra-letra)
#define PIN_LED_X2        27 // Azul (Paso largo entre letras)
#define PIN_LED_Y1        14 // Amarillo (Salto fila intra-letra)
#define PIN_LED_Y2        12 // Naranja (Salto renglón texto)

class HardwareSim {
public:
    void begin() {
        pinMode(PIN_LED_SOLENOIDE, OUTPUT);
        pinMode(PIN_LED_X1, OUTPUT);
        pinMode(PIN_LED_X2, OUTPUT);
        pinMode(PIN_LED_Y1, OUTPUT);
        pinMode(PIN_LED_Y2, OUTPUT);
    }

    // Acciones Atómicas (Simulan el tiempo físico)
    
    void golpear() {
        Serial.print("[SOLENOIDE] Golpe! -> ");
        digitalWrite(PIN_LED_SOLENOIDE, HIGH);
        delay(100); // Tiempo que tarda en bajar
        digitalWrite(PIN_LED_SOLENOIDE, LOW);
        delay(50);  // Tiempo de recuperación
        Serial.println("Recuperado.");
    }

    void moverX_Corto() {
        Serial.println("  [MOTOR X] Moviendo paso CORTO (dentro de letra)");
        digitalWrite(PIN_LED_X1, HIGH);
        delay(200); 
        digitalWrite(PIN_LED_X1, LOW);
        delay(50);
    }

    void moverX_Largo() {
        Serial.println("  [MOTOR X] Moviendo paso LARGO (siguiente letra)");
        digitalWrite(PIN_LED_X2, HIGH);
        delay(300); 
        digitalWrite(PIN_LED_X2, LOW);
        delay(50);
    }

    void moverY_Corto() {
        Serial.println("\n[MOTOR Y] Bajando fila CORTA (mismo renglon)");
        digitalWrite(PIN_LED_Y1, HIGH);
        delay(300);
        digitalWrite(PIN_LED_Y1, LOW);
        delay(100);
    }

    void moverY_Largo() {
        Serial.println("\n\n[MOTOR Y] === SALTO DE LINEA (Nuevo renglon) ===");
        digitalWrite(PIN_LED_Y2, HIGH);
        delay(500);
        digitalWrite(PIN_LED_Y2, LOW);
        delay(100);
    }
    
    // Simula el cambio de dirección (solo log por ahora)
    void setDireccionX(bool izquierdaDerecha) {
        if(izquierdaDerecha) Serial.println(">>> DIREC: IDA (Izquierda -> Derecha) >>>");
        else                 Serial.println("<<< DIREC: VUELTA (Derecha -> Izquierda) <<<");
    }
};

#endif