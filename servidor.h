#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define DEBUG true
#define NO_DEBUG false


class servidor {
  private:
    SoftwareSerial esp8266{10, 11}; // Rx y Tx del arduino
    bool conectado = false;

    String SSID = "RobotDungeon";
    String PASS = "";

    String IP_server = "192.168.1.15";
    String SERV_comando = "";

    void modo_emisor();
    void modo_receptor();

  public:
    String sendData(String command, const int timeout, boolean debug);
    void init();
    String getIp();
    bool connect_modem();
    String play();
    String read_query(boolean debug);

    bool request_server();


};


