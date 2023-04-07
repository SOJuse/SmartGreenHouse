//контроллер №3

#include <ESP8266WiFi.h>
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <Adafruit_AHTX0.h>

#define   WIFI_CHANNEL    6

//*********ПЕРЕМЕННЫЕ*********

//#define sensorPower D8
#define sensorPin A0
#define ledPin 13

//--------PAINLESSMESH--------

#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт по дефолту 5555
//Adafruit_AHTX0 aht; // датчик темп-ры и влажности
Scheduler userScheduler;   // для контроля
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
void sendMessage() ;   //задаем пустышку для коректной работы task

Task taskSendMessage( TASK_SECOND * 5 , TASK_FOREVER, &sendMessage );   //указываем задание
int nodeNumber = 3; //указываем номер ардуинки

//----------------------------

void setup() {
  // pinMode(sensorPower, OUTPUT);
  // Изначально оставляем датчику выключенным
  //  digitalWrite(sensorPower, LOW);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  mesh.setDebugMsgTypes(ERROR | STARTUP );  // установите перед функцией init() чтобы выдавались приветственные сообщения
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, WIFI_CHANNEL);

  //назначаем функциям свои события

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

  userScheduler.addTask(taskSendMessage);   //добавляем задание в обработчик
  taskSendMessage.enable();   //включаем задание
  //  aht.begin();
}

void loop() {
  // она также запустит пользовательский планировщик
  mesh.update();

  // включение светодиода при малом объеме воды
  if (readSensor() < 100){
    digitalWrite(ledPin, HIGH);
  } else {
     digitalWrite(ledPin, LOW);
  }
  }
