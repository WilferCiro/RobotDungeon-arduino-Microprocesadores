#include "bluetooth.h"

void bluetooth::init() {
  Serial1.begin(9600);
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
  if (Serial1.available() > 0) {
    while (Serial1.available() > 0) {
      leido += (char)Serial1.read();
    }
  }
  return leido;
}

void bluetooth::server_send(String comando) {
  Serial1.println(comando);
}

String bluetooth::request_server(String comando) {
  String data = "";
  Serial1.println(comando);
  unsigned long time = millis();
  while (Serial1.available() <= 0 && (time + 4000) > millis()) {
    // esperar sin hacer nada :D
  }
  while (Serial1.available() > 0) {
    char d = Serial1.read();
    data += d;
  }
  return data;
}

