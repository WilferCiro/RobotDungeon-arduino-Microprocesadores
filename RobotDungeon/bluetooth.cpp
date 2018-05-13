#include "bluetooth.h"

void bluetooth::init() {
  hc05.begin(9600);
  conectar_BT();
  close_conection();
}

void bluetooth::conectar_BT() {
  // Comando AT de conexión a bluetooth del PC
}
void bluetooth::close_conection() {
  // Comando AT de desconexión a bluetooth del PC
}

String bluetooth::listen_server() {
  String leido = "";
  if (hc05.available() > 0) {
    while (hc05.available() > 0) {
      leido+=(char)hc05.read();
    }
  }
  return leido;
}

String bluetooth::request_server(String comando) {
  String data = "";
  hc05.println(comando);
  unsigned long time = millis();
  while (hc05.available() <= 0 && (time + 4000) > millis()) {
    // esperar sin hacer nada :D
  }
  while (hc05.available() > 0) {
    char d = hc05.read();
    data += d;
  }
  return data;
}

