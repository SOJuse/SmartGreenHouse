// приёмник на комбинированной плате
#include <ESP8266WiFi.h> 
#include "painlessMesh.h"
#include <Arduino_JSON.h>

//--------PAINLESSMESH--------
#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт по дефолту 5555
#define   WIFI_CHANNEL    6

Scheduler userScheduler;   // планировщик
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
//void sendMessage() ;   //задаем пустышку для коректной работы task
//Task taskSendMessage( TASK_SECOND * 2 , TASK_FOREVER, &sendMessage );   //указываем задание
int nodeNumber; //указываем номер ардуинки
int angle; //угол подъема 
byte doorUp, doorDown;


void setup() {
  
  Serial.begin(115200);
 // mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);  // установите перед функцией init() чтобы выдавались приветственные сообщения
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, WIFI_CHANNEL);

  //назначаем функциям свои события

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

 // userScheduler.addTask(taskSendMessage);   //добавляем задание в обработчик
 // taskSendMessage.enable();   //включаем задание
 
}

void loop() {
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) {
 // Serial.printf(msg.c_str(), "\n");
  JSONVar myObject = JSON.parse(msg.c_str());   //парсим полученные данные
  //записываем значения в переменные
  
  int nodeNumber = myObject["node"];

  if (nodeNumber == 10) {
    Serial.printf(msg.c_str(), "\n");
    }

}

void newConnectionCallback(uint32_t nodeId) {
//  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}
