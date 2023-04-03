#include <Servo.h>
// #include <MsTimer2.h>
#include <Wire.h>               // Подключаем библиотеку Wire 
#include <Adafruit_INA219.h>    // Подключаем библиотеку Adafruit INA219 (A4-SDA-оранж.  A5-SCL-красн.)
#include <Arduino_JSON.h>

#define SERVO1_LEFT 12
#define SERVO2_RIGHT 11
#define CURRENT_SET 850
#define mosPIN 4 //увлажнитель
#define pompPIN1 5 //первая помпа
#define pompPIN2 9 //вторая помпа

// создаём объекты для управления сервоприводами
Servo myservo1;
Servo myservo2;

int angle = 0;
String strData = "";
boolean recievedFlag = false;
Adafruit_INA219 ina219;         // Создаем объект ina219
float current_mA = 0;       // Ток в мА
int TIMER_PERIOD = 20000; //время работы увлажнителя
int TIMER_PERIOD_POLIV = 10000;
byte doorUp = 0; // команды на открытие
byte doorDown = 0; // и закрытие форточки
byte hydration_on = 0; //команда на увлажнение
byte watering_on_1 = 0; //команда на полив первой помпы
byte watering_on_2 = 0; //команда на полив второй помпы
boolean st_up = false; // состояние форточки верх
boolean hum_on = false; // состояние увлажнителя выкл
boolean pomp1_on = false; // состояние первой помпы
boolean pomp2_on = false; // состояние второй помпы
unsigned long cur_time_hum = 0; //время работы увлажнителя
unsigned long cur_time_pomp1 = 0; //время работы первой помпы
unsigned long cur_time_pomp2 = 0; //время работы второй помпы


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
  // полив
  pinMode(pompPIN1, OUTPUT);
  digitalWrite(pompPIN1, 0);
  pinMode(pompPIN2, OUTPUT);
  digitalWrite(pompPIN2, 0);
  delay(1000);
  Serial.begin(115200); // подключаем монитор порта

  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
  }
}

void window_move_up(int angle) {
  // поднимаем вверх
  //  Serial.println("Up!");
  //  Serial.println(angle);

  for (byte i = 0; i <= angle; i++) {
    myservo1.write(0 + i);
    myservo2.write(180 - i);
    // current_mA = ina219.getCurrent_mA();
    //  if (current_mA > CURRENT_SET){ // ПЕРЕГРУЗКА!
    //    break;
    // }
    //   Serial.print("i="); Serial.println(i);
    //   Serial.print("cur="); Serial.println(current_mA);
    delay(50);
  }
}

void window_move_down(int angle) {
  // опускаем вниз
  //  Serial.println("Down!");
  //  Serial.println(angle);

  for (byte i = 0; i <= angle; i++) {
    myservo1.write(angle - i);
    myservo2.write(180 - angle + i);
    //   Serial.print("i="); Serial.println(i);
    delay(50);
  }

}

//void on_timer() {
//digitalWrite(mosPIN, 0); // включить увлажнитель
// MsTimer2::stop();
//}
/*
  void on_timer_poliv() {
  digitalWrite(pompPIN1, 0); // включить увлажнитель
  MsTimer2::stop();
  }
*/

void loop()
{

  if (Serial.available() > 0) {                 // если есть что-то на вход
    strData = "";                               // очистить строку
    while (Serial.available() > 0) { 
    // пока идут данные
      strData += (char)Serial.read();           // получаем данные
      delay(2);    // обязательно задержка, иначе вылетим из цикла раньше времени
      }
    recievedFlag = true;                        // поднять флаг что получили данные
  }


  /*  if (recievedFlag) {                           // если есть принятые данные
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
    }*/

  if (recievedFlag) {
    Serial.println("New data...");
    Serial.println(strData);
    JSONVar myObject = JSON.parse(strData);   //парсим полученные данные
    angle = myObject["an"];
    doorUp = myObject["Up"];
    doorDown = myObject["Dwn"];
    hydration_on = myObject["h_on"];
    watering_on_1 = myObject["w_1"];
    watering_on_2 = myObject["w_2"];

    recievedFlag = false; // данные приняты
    Serial.print("angle = ");
    Serial.println(angle);
    Serial.print("dUp = ");
    Serial.println(doorUp);
    Serial.print("dDown = ");
    Serial.println(doorDown);
    Serial.print("hyd_on = ");
    Serial.println(hydration_on);
    Serial.print("wat_on_1 = ");
    Serial.println(watering_on_1);
    Serial.print("wat_on_2 = ");
    Serial.println(watering_on_2);
  }

  //управлене форточкой
  if (doorUp == 1 && st_up == false) {
    window_move_up(angle);
    st_up = true;
  }

  if (doorDown == 1 && st_up == true) {
    window_move_down(angle);
    st_up = false;
  }

  // управление увлажнителем
  if (hydration_on == 1 && hum_on == false) {
    digitalWrite(mosPIN, 1);
    cur_time_hum = millis();
    hum_on = true;
    Serial.println ("hydration on!");
  }

  if (hum_on && millis() - cur_time_hum >= 10000) {
    digitalWrite(mosPIN, 0);
    hum_on = false;
    Serial.println ("hydration off!");
  }

  // управление поливом

  //управление первой помпой
  if (watering_on_1 == 1 && pomp1_on == false) {
    digitalWrite(pompPIN1, 1);
    cur_time_pomp1 = millis();
    pomp1_on = true;
    Serial.println ("pomp1 on!");
  }

  if (pomp1_on && millis() - cur_time_pomp1 >= 10000) {
    digitalWrite(pompPIN1, 0);
    pomp1_on = false;
    Serial.println ("pomp1 off!");
  }

  //управление второй помпой
  if (watering_on_2 == 1 && pomp2_on == false) {
    digitalWrite(pompPIN2, 1);
    cur_time_pomp2 = millis();
    pomp2_on = true;
    Serial.println ("pomp2 on!");
  }

  if (pomp2_on && millis() - cur_time_pomp2 >= 10000) {
    digitalWrite(pompPIN2, 0);
    pomp2_on = false;
    Serial.println ("pomp2 off!");
  }
}
