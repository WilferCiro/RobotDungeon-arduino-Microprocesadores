#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include "servidor.h"
#include "bluetooth.h"
/*
   PORT MAP
   ---------------------
   | Pin  | Descripción |
   ---------------------
   |20    |Motor1a
   |21    |Motor1b
   |22    |Motor2a
   |23    |Motor2b
   |24    |sensor 1
   |25    |sensor 2
   |26    |s0
   |27    |s1
   |28    |s2
   |29    |s3
   |30    |out
   |31    |Rx
   |32    |Tx

   PWM
   |12    |motor 1 pwm
   |11    |motor 2 pwm
   |10    |Servo
   |9     |Trigger
   |8     |Echo
   |7     |
*/

#define CAMINO 1
#define PARED 2
#define OBJETO 3
#define PUERTA 4

#define DERECHA 1
#define IZQUIERDA 2
#define FRENTE 3
#define ATRAS 4

#define VOLTEANDO 1
#define ANDANDO 2
#define RECOGIENDO 3
#define ESTATICO 3

#define LIBRE 1
#define ESCLAVO 2
#define CIEGO 3

#define MINDISTANCIA 3
class robot {
  private:
    uint8_t pin_motor1a = 20;
    uint8_t pin_motor1b = 21;
    uint8_t pin_motor2a = 22;
    uint8_t pin_motor2b = 23;
    uint8_t pin_motor1_pwm = 12;
    uint8_t pin_motor2_pwm = 11;

    uint8_t pin_servo = 10;
    Servo servo_obj;
    uint8_t servo_arriba = 0;
    uint8_t servo_abajo = 90;

    uint8_t pin_sensor1 = 24;
    uint8_t pin_sensor2 = 25;


    uint8_t TriggerPin = 9;
    uint8_t EchoPin = 8;

    uint8_t s0 = 26;
    uint8_t s1 = 27;
    uint8_t s2 = 28;
    uint8_t s3 = 29;
    uint8_t out = 30;

    uint8_t mapa[13][13] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    uint8_t posX1 = 0;
    uint8_t posX2 = 0;
    uint8_t posY1 = 0;
    uint8_t posY2 = 0;
    bool pasa_sensor1 = false;
    bool pasa_sensor2 = false;
    uint8_t actual_accion = ESTATICO;
    uint8_t actual_direccion = FRENTE;

    uint8_t accion_global = LIBRE;

    volatile const int timeThreshold = 100;
    volatile long timeCounter1 = 0;
    volatile long timeCounter2 = 0;
    servidor server;
    bluetooth BT;

    int contador = 0; // Importante por la interrupción
    int contador_volteo = 0;
    int contador_recoger = 0;

    uint8_t fase_volteo = 1;
    bool pared_frente = false;

    char color_tengo = 'N';

  public:
    void andar(uint8_t);
    void frenar();
    void voltear(char);
    void levantar_garra();
    void bajar_garra();
    void init();

    void play();

    void explore_libre();


    char sensar_color();
    int sensar_sonido();

    void guarda_mapa(uint8_t posx, uint8_t posy, char tipo);
    
    void sensor_1();
    void sensor_2();
    void administra_comando(String);
    void aumenta_contadores();

    void actualiza_direccion();

    void calibrar_motores();
    void explore_ciego();

};
