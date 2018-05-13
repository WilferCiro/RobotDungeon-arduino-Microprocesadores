#include "servidor.h"

void servidor::init() {
  esp8266.begin(9600);
  connect_modem();
  getIp();
  modo_receptor();
}

String servidor::play() {
  String com = read_query(DEBUG);

  SERV_comando = com;
  //Serial.println(com);

  return SERV_comando;
}

String servidor::read_query(boolean debug) {
  String query = "";
  //modo_receptor();
  if (esp8266.available()) {
    unsigned long time = millis();
    //  while (!esp8266.find("+IPD,") && (time + 7000) > millis()) {
    //    // esperar sin hacer nada :D
    //  }
    if (esp8266.find("+IPD,")) {
      String msg;
      esp8266.find("?");
      msg = esp8266.readStringUntil(' ');
      String command1 = msg.substring(0, 3);
      String command2 = msg.substring(4);

      if (debug) {
        Serial.println(command1);//debe mostrar "COM"
        Serial.println(command2);//debe mostrar Tipo de comando
      }
      query = command2;
      //query = "So";
    }
    else {
      Serial.println("Horror");
    }
  }
  else {
    //Serial.println("Error en el +IPD,");
  }

  return query;
}


bool servidor::connect_modem() {
  bool connect = false;
  Serial.println("Conectando el modem...");
  sendData("AT+CWMODE=3\r\n", 1000, DEBUG);
  String retorna = sendData("AT+CWJAP=\"" + SSID + "\",\"" + PASS + "\"\r\n", 4000, DEBUG);
  if (retorna.indexOf("OK") != -1) {
    connect = true;
  }
  return true;
}

String servidor::getIp() {
  String IP = sendData("AT+CIFSR\r\n", 1000, DEBUG);
  return IP;
}

String servidor::sendData(String command, const int timeout, boolean debug) {
  String response = "";
  long int time = 0;
  esp8266.println(command); // Enviar datos al módulo esp8266
  time = millis(); // Obtener los miliseguntos actuales
  while ((time + timeout) > millis()) {
    while (esp8266.available()) {
      char c = esp8266.read(); // Leer el caracter del buffer.
      response += c; // concatenar todos los caracteres del buffer
    }
  }
  if (debug) {
    Serial.println(response); // Si se desea debug, mostrar en el monitor serial
  }
  return response;
}





bool servidor::request_server() {
  bool valido = false;

  modo_emisor();

  esp8266.println("AT+CIPSTART=\"TCP\",\"" + IP_server + "\",80");

  if (esp8266.find("OK")) {
    //Armamos el encabezado de la peticion http
    String peticionHTTP = "GET /www/robots/nuevo.php?a=";
    peticionHTTP = peticionHTTP + String(50) + "&b=" + String(20) + " HTTP/1.1\r\n";
    peticionHTTP = peticionHTTP + "Host: " + IP_server + "\r\n\r\n";

    //Enviamos el tamaño en caracteres de la peticion http:
    esp8266.print("AT+CIPSEND=");
    esp8266.println(peticionHTTP.length());

    //esperamos a ">" para enviar la petcion  http
    // ">" indica que podemos enviar la peticion http
    if (esp8266.find(">")) {
      Serial.println("Enviando HTTP . . .");
      esp8266.println(peticionHTTP);
      if (esp8266.find("SEND OK")) {
        esp8266.println("AT+CIPCLOSE");
        if (esp8266.find("OK")) {
          Serial.println("Peticion HTTP enviada:");
          Serial.println();
          Serial.println(peticionHTTP);
          valido = true;
        }
        /*Serial.println("Peticion HTTP enviada:");
          Serial.println();
          Serial.println(peticionHTTP);
          Serial.println("Esperando respuesta...");

          boolean fin_respuesta = false;
          long tiempo_inicio = millis();
          String cadena = "";

          while (fin_respuesta == false) {
          while (esp8266.available() > 0) {
            char c = esp8266.read();
            cadena.concat(c);  //guardamos la respuesta en el string "cadena"
          }
          //finalizamos si la respuesta es mayor a 500 caracteres
          //Pueden aumentar si tenen suficiente espacio en la memoria
          if (cadena.length() > 500) {
            Serial.println("La respuesta a excedido el tamaño maximo");
            esp8266.println("AT+CIPCLOSE");
            if (esp8266.find("OK")) {
              Serial.println("Conexion finalizada");
            }
            valido = false;
            fin_respuesta = true;
          }
          //Finalizamos si ya han transcurrido 10 seg
          if ((millis() - tiempo_inicio) > 10000) {
            Serial.println("Tiempo de espera agotado");
            esp8266.println("AT+CIPCLOSE");
            if (esp8266.find("OK")) {
              Serial.println("Conexion finalizada");
            }
            valido = false;
            fin_respuesta = true;
          }
          //si recibimos un CLOSED significa que ha finalizado la respuesta
          if (cadena.indexOf("CLOSED") > 0) {
            Serial.println();
            Serial.println("Cadena recibida correctamente, conexion finalizada");
            fin_respuesta = true;
            valido = true;
          }
          }*/
      }
      else {
        Serial.println("No se ha podido enviar HTTP.....");
      }
    }
    else {
      Serial.println("No se ha podido enviar la inicialización.....");
    }
  }
  else {
    Serial.println("No se ha podido conectarse con el servidor");
  }
  return valido;
}


void servidor::modo_emisor() {
  sendData("AT+CIPSERVER=0\r\n", 1000, DEBUG); // turn off server on port 80
  sendData("AT+CIPMUX=0\r\n", 1000, DEBUG); // configure for unica conexion
}
void servidor::modo_receptor() {
  sendData("AT+CIPMUX=1\r\n", 1000, NO_DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, NO_DEBUG); // turn on server on port 80
}
