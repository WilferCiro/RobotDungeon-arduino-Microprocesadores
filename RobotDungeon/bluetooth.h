#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

class bluetooth {
  private:
    SoftwareSerial hc05{2, 3}; // Rx y Tx del arduino
  public:
    void init();
    void play();
    String request_server(String comando);
    void close_conection();
    void conectar_BT();
};
