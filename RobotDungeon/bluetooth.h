#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

class bluetooth {
  private:
    SoftwareSerial hc05{31, 32}; // Rx y Tx del arduino
  public:
    void init();
    void play();
    String request_server(String comando);
    void close_conection();
    void conectar_BT();
    String listen_server();
};
