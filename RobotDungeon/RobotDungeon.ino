/*
   Código para el esclavo
*/

/*
   TODO:
     cambiar guarda_mapa
     agregar camino_corto
     Hacer Maestro
     Implementar bailar()
*/

/*
   PORT MAP
   ---------------------
   | Pin  | Descripción |
   ---------------------
   |20    |Motor1a
   |21    |Motor1b
   |22    |Motor2a
   |23    |Motor2b
   |18    |sensor 1 -> CNY
   |19    |sensor 2 -> CNY
   |26    |s0
   |24    |s1
   |25    |s2
   |29    |s3
   |30    |out
   |Serial1|Rx
   |Serial1|Tx

   PWM
   |12    |motor 1 pwm
   |11    |motor 2 pwm
   |10    |Servo
   |9     |Trigger
   |8     |Echo
   |7     |
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

  uint8_t PinCNY70_1 = 18;
  uint8_t PinCNY70_2 = 19;
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
