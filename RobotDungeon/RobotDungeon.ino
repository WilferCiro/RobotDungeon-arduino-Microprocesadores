/*
   Código para el esclavo
*/

#include "robot.h"

volatile robot rob;

void setup() {
  rob.init();
  //Modo CTC y preescalado de 1024
  TCCR2A = (1 << WGM21);
  TCCR2B = (7 << CS20);
  OCR2A = 156;
  // Activa la interrución por comparación
  TIMSK2 = (1 << OCIE2A);

  uint8_t PinCNY70_1 = 24;
  uint8_t PinCNY70_2 = 25;
  attachInterrupt(digitalPinToInterrupt(PinCNY70_1), sensor_1, FALLING);
  attachInterrupt(digitalPinToInterrupt(PinCNY70_2), sensor_2, FALLING);
}

void loop() {
  rob.play();
}


ISR(TIMER2_COMPA_vect) {
  rob.aumenta_contadores();
}

void sensor_1() {
  rob.sensor_1();
}
void sensor_2() {
  rob.sensor_2();
}
