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

  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

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
  String comando = BT.listen_server();
  if (comando != "") {
    comando_bt = comando;
    contador_volteo = 0;
    contador_recoger = 0;
  }

  if (pasa_sensor1 == true && pasa_sensor2 == true && actual_accion == ANDANDO) {
    pasa_sensor1 = pasa_sensor2 = false;
    termina_accion_esclavo();
  }

  if (actual_accion == VOLTEANDO) {
    if (contador_volteo >= 50) {
      termina_accion_esclavo();
    }
  }
  else if (actual_accion == RECOGIENDO) {
    if (contador_recoger >= 20) {
      termina_accion_esclavo();
    }
  }
  administra_comando();
  contador = 0;
}

void robot::termina_accion_esclavo() {
  comando_bt = "";
  BT.server_send("Fin");
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
        guarda_mapa(posX1, posY1, CAMINO);
      }
      else if (color == 'G') {
        guarda_mapa(posX1, posY1, PUERTA);
      }
      else {
        guarda_mapa(posX1, posY1, PARED);
      }
    }
    else {
      pared_frente = false;
      guarda_mapa(posX1, posY1, CAMINO);
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
          guarda_mapa(posX1, posY1, PUERTA);
        }
        else {
          guarda_mapa(posX1, posY1, PARED);
        }
      }
      else {
        actual_accion = ESTATICO;
        frenar();
        fase_volteo = 1;
        guarda_mapa(posX1, posY1, CAMINO);
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
      BT.server_send("Ten-" + color_tengo);
      enviar_mapa();
      actual_accion = ESTATICO;
    }
  }
}

void robot::enviar_mapa() {
  for (uint8_t i = 0; i < 10; i++) {
    for (uint8_t a = 0; a < 10; a++) {
      delay(10);
      BT.server_send("Map-" +i+"-"+a+"-"+mapa[i][a]);
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

void robot::administra_comando() {
  String tipo = comando_bt.substring(0, 3);
  String resto = comando_bt.substring(5);
  if (tipo == "Cam") { // Camine
    andar(0);
    actual_accion = ANDANDO;
  }
  else if (tipo == "Gir") { // Gire
    if (resto == "D") {
      voltear('D');
    }
    else {
      voltear('I');
    }
    actual_accion = VOLTEANDO;
  }
  else if (tipo == "Abr") {
    levantar_garra();
    actual_accion = RECOGIENDO;
  }
  else if (tipo == "Cer") {
    bajar_garra();
    actual_accion = RECOGIENDO;
  }
  else if (tipo == "Bai") {
    //bailar();
    actual_accion = BAILANDO;
  }
  else { // comando=Lib o sin comando
    frenar();
    actual_accion = ESTATICO;
  }
}


void robot::andar(uint8_t cual_motor) {
  if (cual_motor == 0 || cual_motor == IZQUIERDA) {
    digitalWrite(pin_motor1a, HIGH);
    digitalWrite(pin_motor1b, LOW);
    analogWrite(pin_motor1_pwm, PWM_MOTOR1);
  }
  if (cual_motor == 0 || cual_motor == DERECHA) {
    digitalWrite(pin_motor2a, HIGH);
    digitalWrite(pin_motor2b, LOW);
    analogWrite(pin_motor2_pwm, PWM_MOTOR2);
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


void robot::guarda_mapa(uint8_t posx, uint8_t posy, uint8_t tipo) {
  if (tipo == CAMINO) { // Camino
    mapa[posx * 2][posy * 2] = CAMINO;
  }
  else if (tipo == PARED) { // Muro
    mapa[(posx * 2) + 1][(posy * 2) + 1] = PARED;
  }
  else if (tipo == OBJETO) { // Objeto
    mapa[posx * 2][posy * 2] = OBJETO;
  }
  else if (tipo == PUERTA) { // Puerta
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
