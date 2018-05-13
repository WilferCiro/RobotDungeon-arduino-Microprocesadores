#include "robot.h"

void robot::init() {
  Serial.begin(9600);
  pinMode(pin_motor1a, OUTPUT);
  pinMode(pin_motor1b, OUTPUT);
  pinMode(pin_motor2a, OUTPUT);
  pinMode(pin_motor2b, OUTPUT);
  pinMode(pin_motor1_pwm, OUTPUT);
  pinMode(pin_motor2_pwm, OUTPUT);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);

  servo_obj.attach(pin_servo);
  levantar_garra();
  frenar();

  BT.init();
  server.init();

}

void robot::play() {
  if (accion_global == LIBRE) {
    explore_libre();
  }
  else if (accion_global == CIEGO) {
    explore_ciego();
  }
  else {
    Serial.println("Error en la acción global");
  }
}

void robot::explore_ciego() {
  if (contador >= 10) {
    String comando = BT.listen_server();
    if (comando != "") {
      administra_comando(comando);
    }
    contador = 0;
  }
}

void robot::explore_libre() {
  if (pasa_sensor1 == true && pasa_sensor2 == true) {
    pasa_sensor1 = pasa_sensor2 = false;
    actual_accion = VOLTEANDO;
    if (sensar_sonido() < MINDISTANCIA) {
      pared_frente = true;
      char color = sensar_color();
      if (color != 'N' && color != 'G') {
        color_tengo = color;
        actual_accion = RECOGIENDO;
        contador_recoger = 0;
      }
      else if (color == 'G') {
        // guarda_mapa(posX, posY, PUERTA);
      }
      else {
        //guarda_mapa(posX, posY, PARED);
      }
    }
    else {
      pared_frente = false;
      //guarda_mapa(posX, posY, CAMINO);
    }
    fase_volteo = 1;
    contador_volteo = 0;
  }


  if (actual_accion == VOLTEANDO) {
    if (fase_volteo == 1) {
      voltear('D');
    }
    else {
      voltear('I');
    }
    if (contador_volteo >= 50) {  // TODO: Cambiar Nro
      actualiza_direccion();

      if (sensar_sonido() < MINDISTANCIA) { // TODO: revisar la distancia a la que queda de la pared aproximadamente :D
        fase_volteo++;

        char color = sensar_color();
        if (color != 'N' && color != 'G') {
          color_tengo = color;
          actual_accion = RECOGIENDO;
          contador_recoger = 0;
        }
        else if (color == 'G') {
          // guarda_mapa(posX, posY, PUERTA);
        }

        //guarda_mapa(posX, posY, PARED);
      }
      else {
        actual_accion = ESTATICO;
        frenar();
        fase_volteo = 1;
        //guarda_mapa(posX, posY, CAMINO);
      }
      contador_volteo = 0;
      // Leer sensores :v
    }
  }
  else if (actual_accion == ANDANDO) {
    if ((posX1 != posX2) || (posY1 != posY2)) {
      calibrar_motores();
    }
    else {
      if (sensar_sonido() > MINDISTANCIA) {
        andar(0);
      }
    }
  }
  else if (actual_accion == RECOGIENDO) {
    bajar_garra();
    if (contador_recoger >= 20) {
      accion_global = CIEGO;
      contador_recoger = 0;
    }
  }
}

void robot::actualiza_direccion() {
  if (fase_volteo == 1) {
    if (actual_direccion == FRENTE) {
      actual_direccion = DERECHA;
    }
    else if (actual_direccion == DERECHA) {
      actual_direccion = ATRAS;
    }
    else if (actual_direccion == ATRAS) {
      actual_direccion = IZQUIERDA;
    }
    else if (actual_direccion == IZQUIERDA) {
      actual_direccion = FRENTE;
    }
  }
  else {
    if (actual_direccion == FRENTE) {
      actual_direccion = IZQUIERDA;
    }
    else if (actual_direccion == IZQUIERDA) {
      actual_direccion = ATRAS;
    }
    else if (actual_direccion == ATRAS) {
      actual_direccion = DERECHA;
    }
    else if (actual_direccion == DERECHA) {
      actual_direccion = FRENTE;
    }
  }
}



void robot::calibrar_motores() {
  if ((posX1 > posX2) || (posY1 > posY2)) {
    if (actual_direccion == FRENTE || actual_direccion == DERECHA) {
      // Mover derecha
      andar(DERECHA);
    }
    else if (actual_direccion == ATRAS || actual_direccion == IZQUIERDA) {
      // Mover izquierda
      andar(IZQUIERDA);
    }
  }
  else if ((posX1 < posX2) || (posY1 < posY2)) {
    if (actual_direccion == FRENTE || actual_direccion == DERECHA) {
      // Mover izquierda
      andar(IZQUIERDA);
    }
    else if (actual_direccion == ATRAS || actual_direccion == IZQUIERDA) {
      // Mover derecha
      andar(DERECHA);
    }
  }

}


void robot::administra_comando(String comando) {
  String tipo = comando.substring(0, 3);
  if (tipo == "Ten") {
    Serial.println("Lola");
  }
  else if (tipo == "") {

  }
  else { // comando=Lib o sin comando
    Serial.println("Lola");
  }
}


void robot::andar(uint8_t cual_motor) {
  if (cual_motor == 0 || cual_motor == IZQUIERDA) {
    digitalWrite(pin_motor1a, HIGH);
    digitalWrite(pin_motor1b, LOW);
  }
  if (cual_motor == 0 || cual_motor == DERECHA) {
    digitalWrite(pin_motor2a, HIGH);
    digitalWrite(pin_motor2b, LOW);
  }
}
void robot::frenar() {
  digitalWrite(pin_motor1a, LOW);
  digitalWrite(pin_motor1b, LOW);
  digitalWrite(pin_motor2a, LOW);
  digitalWrite(pin_motor2b, LOW);
}
void robot::voltear(char direccion) {
  switch (direccion) {
    case 'D':
      digitalWrite(pin_motor1a, HIGH);
      digitalWrite(pin_motor1b, LOW);
      digitalWrite(pin_motor2a, LOW);
      digitalWrite(pin_motor2b, HIGH);
      break;
    case 'I':
      digitalWrite(pin_motor1a, LOW);
      digitalWrite(pin_motor1b, HIGH);
      digitalWrite(pin_motor2a, HIGH);
      digitalWrite(pin_motor2b, LOW);
      break;
    default:
      frenar();
      break;
  }
}
void robot::levantar_garra() {
  servo_obj.write(servo_arriba);
}
void robot::bajar_garra() {
  servo_obj.write(servo_abajo);
}

char robot::sensar_color() {
  char color = 'N';
  byte countRed = 0;
  byte countGreen = 0;
  byte countBlue = 0;
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  countRed = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s3, HIGH);
  countBlue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s2, HIGH);
  countGreen = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  if (countRed < countBlue && countGreen > 100 && countRed < 80) {
    color = 'R';
  }
  else if (countBlue < countRed && countBlue < countGreen) {
    color = 'G';
  }
  else if (countGreen < countRed && countGreen < countBlue) {
    color = 'B';
  }
  return color;
}
int robot::sensar_sonido() {
  int duration, distancia_cm;
  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  distancia_cm = (duration * 10) / (292 * 2);  //convertimos a distancia, en cm
  return distancia_cm;
}


void robot::guarda_mapa(uint8_t posx, uint8_t posy, char tipo) {
  if (tipo == 'C') { // Camino
    mapa[posx * 2][posy * 2] = CAMINO;
  }
  else if (tipo == 'M') { // Muro
    mapa[(posx * 2) + 1][(posy * 2) + 1] = PARED;
  }
  else if (tipo == 'O') { // Objeto
    mapa[posx * 2][posy * 2] = OBJETO;
  }
  else if (tipo == 'P') { // Puerta
    mapa[posx * 2][posy * 2] = PUERTA;
  }
}


void robot::sensor_2() {
  if (millis() > timeCounter2 + timeThreshold) {
    timeCounter2 = millis();
    if (actual_accion == ANDANDO) {
      switch (actual_direccion) {
        case FRENTE:
          posY2 = posY2 + 1;
          break;
        case ATRAS:
          posY2 = posY2 - 1;
          break;
        case DERECHA:
          posX2 = posX2 + 1;
          break;
        case IZQUIERDA:
          posX2 = posX2 - 1;
          break;
        default:
          break;
      }
    }
    pasa_sensor2 = true;
  }
}

void robot::sensor_1() {
  if (millis() > timeCounter2 + timeThreshold) {
    timeCounter2 = millis();
    if (actual_accion == ANDANDO) {
      switch (actual_direccion) {
        case FRENTE:
          posY1 = posY1 + 1;
          break;
        case ATRAS:
          posY1 = posY1 - 1;
          break;
        case DERECHA:
          posX1 = posX1 + 1;
          break;
        case IZQUIERDA:
          posX1 = posX1 - 1;
          break;
        default:
          break;
      }
    }
    pasa_sensor1 = true;
  }
}

void robot::aumenta_contadores() {
  contador ++;
  contador_volteo ++;
  contador_recoger ++;
}
