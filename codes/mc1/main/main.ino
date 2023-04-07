//контроллер № 1

#include <ESP8266WiFi.h> 
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <Adafruit_AHTX0.h>

#define NOSENSORS 0 // тестовые данные без датчиков задать 1
#define   WIFI_CHANNEL    1
#define ledPin D13

//*********ПЕРЕМЕННЫЕ*********

//#define sensorPower D2
#define sensorPin A0

//--------PAINLESSMESH--------

#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт по дефолту 5555
Adafruit_AHTX0 aht; // датчик темп-ры и влажности
Scheduler userScheduler;   // планировщик
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
void sendMessage() ;   //задаем пустышку для корректной работы task
Task taskSendMessage( TASK_SECOND * 2 , TASK_FOREVER, &sendMessage );   //указываем задание
byte nodeNumber = 1; //указываем номер ардуинки
//----------------------------
int angle = 42; //угол подъема по умолчанию
int set_hydration = 10; //уставка влажности по умолчанию
int set_temperature = 30; //уставка температуры по умолчанию
//double water;
double temp1 = 15; //начальная температура, чтобы не было сработки
double hum = 50; //начальная влажность, чтобы не было сработки
byte ghum, doorUp, doorDown, hydration_on, watering_on_1, watering_on_2;
long int hum_time; //время, в которое запускается увлжанитель
long int hum_time_2; //время, в которое отключаем увлжанитель
bool hum_flag = 1; //флаг, отвечающий за активность увлажнителя
int set_gh = 1; //уставка влажности почвы по умолчанию

void setup() {
//  pinMode(sensorPower, OUTPUT);
  // Изначально оставляем датчик выключенным
//  digitalWrite(sensorPower, LOW);
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  mesh.setDebugMsgTypes(ERROR | STARTUP );  // установите перед функцией init() чтобы выдавались приветственные сообщения
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, WIFI_CHANNEL);

  //назначаем функциям свои события

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

  userScheduler.addTask(taskSendMessage);   //добавляем задание в обработчик
  taskSendMessage.enable();   //включаем задание
  aht.begin();
}

void loop() {
  // она также запустит пользовательский планировщик
  mesh.update();
}

void autoControl() {
 
  //управление влажностью
  if (hum < set_hydration && hydration_on == 0 && hum_flag == 1) {
    hydration_on = 1;
    hum_time = millis();
  }
  else if (hydration_on == 1) {
    if (millis() - hum_time >= 10000) {
      hydration_on = 0;
      hum_time_2 = millis();
      hum_flag = 0;
    }
  }
  if (hum_flag == 0) {
    if (millis() - hum_time_2 >= 3000) {
      hum_flag = 1;
    }
  }
  //управление форточкой

  if (temp1 > set_temperature) {
    Serial.print("doorUp=");
    Serial.println(doorUp);
    doorUp = 1;
    doorDown = 0;
  } 
  else {
    doorUp = 0;
    doorDown = 1;
  }

  //управление поливом
  if (ghum < set_gh) {
    watering_on_1 = 1;
  } else {
    watering_on_1 = 0;
  }

 
 
}
