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
   |
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
    uint8_t pin_motor1_pwm = 20;
    uint8_t pin_motor2_pwm = 20;
    uint8_t pin_motor2a = 22;
    uint8_t pin_motor2b = 23;

    uint8_t pin_servo = 3;
    Servo servo_obj;
    uint8_t servo_arriba = 0;
    uint8_t servo_abajo = 90;

    uint8_t pin_sensor1 = A0;
    uint8_t pin_sensor2 = A1;


    uint8_t TriggerPin = 6;
    uint8_t EchoPin = 5;

    uint8_t s0 = 8;
    uint8_t s1 = 9;
    uint8_t s2 = 12;
    uint8_t s3 = 11;
    uint8_t out = 10;

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

};
