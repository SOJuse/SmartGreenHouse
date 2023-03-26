//контроллер №2

#include <ESP8266WiFi.h>
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <Adafruit_AHTX0.h>


//*********ПЕРЕМЕННЫЕ*********

//#define sensorPower D2
#define sensorPin A0
#define NOSENSORS 1 // тестовые данные без датчиков задать 1
#define   WIFI_CHANNEL    8

//--------PAINLESSMESH--------

#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт по дефолту 5555
Adafruit_AHTX0 aht; // датчик темп-ры и влажности
Scheduler userScheduler;   // для контроля
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
void sendMessage() ;   //задаем пустышку для коректной работы task
Task taskSendMessage( TASK_SECOND * 2 , TASK_FOREVER, &sendMessage );   //указываем задание
int nodeNumber = 2; //указываем номер ардуинки

//----------------------------

void setup() {
 // pinMode(sensorPower, OUTPUT);
  // Изначально оставляем датчику выключенным
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
