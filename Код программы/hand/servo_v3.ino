#include <Servo.h>
#include <MsTimer2.h>
#include <Wire.h>               // Подключаем библиотеку Wire 
#include <Adafruit_INA219.h>    // Подключаем библиотеку Adafruit INA219 (A4-SDA-оранж.  A5-SCL-красн.)
#define SERVO1_LEFT 12
#define SERVO2_RIGHT 11
#define CURRENT_SET 850
#define mosPIN 4
#define pompPIN1 9

// создаём объекты для управления сервоприводами
Servo myservo1;
Servo myservo2;

boolean start_up = false;
boolean start_down = false;
int angle = 0;
String strData = "";
boolean recievedFlag = false;
Adafruit_INA219 ina219;         // Создаем объект ina219
float current_mA = 0;       // Ток в мА
int TIMER_PERIOD = 20000; //время работы увлажнителя
int TIMER_PERIOD_POLIV = 10000;

void setup()
{
  // подключаем сервоприводы к выводам 11 и 12
  myservo1.attach(SERVO1_LEFT);
  myservo2.attach(SERVO2_RIGHT);
  myservo1.write(0);
  myservo2.write(180);
  // увлажнитель
  pinMode(mosPIN, OUTPUT);
  digitalWrite(mosPIN, 0);
  MsTimer2::set(TIMER_PERIOD, on_timer);
  MsTimer2::set(TIMER_PERIOD_POLIV, on_timer_poliv);

  delay(1000);
  Serial.begin(9600); // подключаем монитор порта

  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
  }
}

void window_move_up(int angle) {
  // поднимаем вверх
  Serial.println("Up!");
  Serial.println(angle);

  for (byte i = 0; i <= angle; i++) {
    myservo1.write(0 + i);
    myservo2.write(180 - i);
    // current_mA = ina219.getCurrent_mA();
    //  if (current_mA > CURRENT_SET){ // ПЕРЕГРУЗКА!
    //    break;
    // }
    Serial.print("i="); Serial.println(i);
    Serial.print("cur="); Serial.println(current_mA);
    delay(50);
  }
}

void window_move_down(int angle) {
  // опускаем вниз
  Serial.println("Down!");
  Serial.println(angle);

  for (byte i = 0; i <= angle; i++) {
    myservo1.write(angle - i);
    myservo2.write(180 - angle + i);
    Serial.print("i="); Serial.println(i);
    delay(50);
  }

}

void on_timer() {
  digitalWrite(mosPIN, 0); // включить увлажнитель
  MsTimer2::stop();
}

void on_timer_poliv() {
  digitalWrite(pompPIN1, 0); // включить увлажнитель
  MsTimer2::stop();
}

void loop()
{

  if (Serial.available() > 0) {                 // если есть что-то на вход
    strData = "";                               // очистить строку
    while (Serial.available() > 0) {            // пока идут данные
      strData += (char)Serial.read();           // получаем данные
      delay(2);                                 // обязательно задержка, иначе вылетим из цикла раньше времени
    }
    recievedFlag = true;                        // поднять флаг что получили данные
  }
  if (recievedFlag) {                           // если есть принятые данные
    if (strData.startsWith("up")) {              // разбор строки
      start_up = true;
      Serial.println("up");
      Serial.println(start_up);
    }
    else if (strData.startsWith("down")) {              //
      start_down = true;
    }
    else if (strData.startsWith("on")) {              //
      digitalWrite(mosPIN, 1);
      MsTimer2::start();

    }
    else if (strData.startsWith("off")) {              //
      digitalWrite(mosPIN, 0); // выключить увлажнитель
    }

    else if (strData.startsWith("poliv")) {              //
      digitalWrite(pompPIN1, 1); // включить насос
      MsTimer2::start();

    }
    else {
      angle = strData.toInt();
    }
    recievedFlag = false;                       // данные приняты!
  }



//Serial.println(start_stop);
if (start_up) {
  window_move_up(angle);
  start_up = false;
}

if (start_down) {
  window_move_down(angle);
  start_down = false;
}

}
