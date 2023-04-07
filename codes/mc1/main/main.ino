//контроллер № 1

#include <ESP8266WiFi.h> 
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <Adafruit_AHTX0.h>

#define NOSENSORS 1 // тестовые данные без датчиков задать 1
#define   WIFI_CHANNEL    8

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
void sendMessage() ;   //задаем пустышку для коректной работы task
Task taskSendMessage( TASK_SECOND * 2 , TASK_FOREVER, &sendMessage );   //указываем задание
byte nodeNumber = 1; //указываем номер ардуинки
//----------------------------
int angle = 30; //угол подъема по умолчанию
int set_hydration = 30; //уставка влажности по умолчанию
int set_temperature = 30; //уставка температуры по умолчанию
//double water;
double temp = 15; //начальная температура, чтобы не было сработки
double temp1;
double hum = 50; //начальная влажность, чтобы не было сработки
double hum1;
byte ghum1, doorUp, doorDown, hydration_on, watering_on_1, watering_on_2;
//String s_ghum1, s_ghum2;

void setup() {
//  pinMode(sensorPower, OUTPUT);
  // Изначально оставляем датчик выключенным
//  digitalWrite(sensorPower, LOW);
  Serial.begin(115200);
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
