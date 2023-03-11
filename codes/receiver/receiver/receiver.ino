// приёмник на комбинированной плате
#include <ESP8266WiFi.h> 
#include "painlessMesh.h"
#include <Arduino_JSON.h>

//--------PAINLESSMESH--------
#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт по дефолту 5555

Scheduler userScheduler;   // планировщик
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
//void sendMessage() ;   //задаем пустышку для коректной работы task
//Task taskSendMessage( TASK_SECOND * 2 , TASK_FOREVER, &sendMessage );   //указываем задание
int nodeNumber; //указываем номер ардуинки
int angle; //угол подъема 
//double temp, temp1, temp2, water;
//double hum, hum1, hum2;
byte doorUp, doorDown;
//String s_ghum1, s_ghum2;

void setup() {
  
  Serial.begin(115200);
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);  // установите перед функцией init() чтобы выдавались приветственные сообщения
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );

  //назначаем функциям свои события

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

 // userScheduler.addTask(taskSendMessage);   //добавляем задание в обработчик
 // taskSendMessage.enable();   //включаем задание
 Serial.println("Start...");
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
  /*
  switch (nodeNumber) {
    case 1:
      temp1 = myObject["temp"];
      hum1 = myObject["hum"];
      ghum1 = myObject["ghum"];
      if (ghum1 == 0) s_ghum1 = "Dry";
      if (ghum1 == 1) s_ghum1 = "Norm";
      if (ghum1 == 2) s_ghum1 = "Wet";
      break;
    case 2:
      temp2 = myObject["temp"];
      hum2 = myObject["hum"];
      ghum2 = myObject["ghum"];
      if (ghum2 == 0) s_ghum2 = "Dry";
      if (ghum2 == 1) s_ghum2 = "Norm";
      if (ghum1 == 2) s_ghum2 = "Wet";
      break;
    case 3:
      water = myObject["waterLevel"];
      break;
    case 10:
      angle = myObject["angle"];
      doorUp = myObject["doorUp"];
      doorUp = doorDown["doorDown"];
  }
  temp = (temp1 + temp2) / 2;
  hum = (hum1 + hum2) / 2;
  Serial.print("MK1 ");
  Serial.println(s_ghum1);
  Serial.print("MK2 ");
  Serial.println(s_ghum2);
  Serial.print("MK3 ");
  Serial.println(water);
  Serial.print("T1=");
  Serial.println(temp1);
  Serial.print("T2=");
  Serial.println(temp2);
  Serial.print("Tavg=");
  Serial.println(temp);
  Serial.print("h1=");
  Serial.println(hum1);
  Serial.print("h2=");
  Serial.println(hum2);
  Serial.print("havg=");
  Serial.println(hum);
  Serial.println("***********************");
  Serial.print("angle=");
  Serial.println(angle);
  Serial.print("doorUp=");
  Serial.println(doorUp);
  Serial.print("doorDown=");
  Serial.println(doorDown);
  Serial.println("<**********************>");
  */
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}
