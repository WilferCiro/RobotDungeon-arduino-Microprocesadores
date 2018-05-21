#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

class bluetooth {
  private:
  public:
    void init();
    void play();
    String request_server(String);
    void close_conection();
    void conectar_BT();
    String listen_server();
    void server_send(String);
};
