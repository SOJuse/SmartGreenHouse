// приёмник на комбинированной плате
#include <ESP8266WiFi.h>
#include "painlessMesh.h"
#include <Arduino_JSON.h>

//--------PAINLESSMESH--------
#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт по дефолту 5555
#define   WIFI_CHANNEL    1
#define   COUNTER_LIM    6 // предел счетчика

Scheduler userScheduler;   // планировщик
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
void countConnection() ;   //задаем пустышку для корректной работы countConnection
Task taskcountConnection( TASK_SECOND * 5 , TASK_FOREVER, &countConnection );   //указываем задание
void blinker() ;   //задаем пустышку для корректной работы blinker
Task taskblinker( TASK_SECOND * 1 , TASK_FOREVER, &blinker );   //указываем задание

int nodeNumber; // номер ардуинки
int angle; //угол подъема
byte doorUp, doorDown, hydration_on, watering_on_1, watering_on_2;
byte counter_10 = 0; // счетчики для проверки связи с платами 10,1 и 2
byte counter_1 = 0;
byte counter_2 = 0;
boolean led = true;

void setup() {

  Serial.begin(115200);
  // mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);  // установите перед функцией init() чтобы выдавались приветственные сообщения
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, WIFI_CHANNEL);

  //назначаем функциям свои события

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

  userScheduler.addTask(taskcountConnection);   //добавляем задание в обработчик
  taskcountConnection.enable();   //включаем задание
  userScheduler.addTask(taskblinker);   //добавляем задание в обработчик
  taskblinker.enable();   //включаем задание
  
  pinMode(BUILTIN_LED, OUTPUT); // индикатор подключения
  digitalWrite(BUILTIN_LED, LOW); // включаем led
}

void loop() {
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) {
  // Serial.printf(msg.c_str(), "\n");
  JSONVar myObject = JSON.parse(msg.c_str());   //парсим полученные данные
  JSONVar jsonSend;
  
  nodeNumber = myObject["nd"];
    // если есть сообщение от платы, прибавляем её счетчик
    switch (nodeNumber) {
      case 1:
      if (counter_1 < COUNTER_LIM){
      counter_1 = counter_1 + 2;
      }
      break;
      case 2:
      if (counter_2 < COUNTER_LIM){
      counter_2 = counter_2 + 2;
      }
      break;
      case 10:
      if (counter_10 < COUNTER_LIM){
      counter_10 = counter_10 + 2;
      }
      break;
    }
  
  if (counter_10 > 0 && nodeNumber == 10){ // если шлюз передает, берем команды от него
  //записываем значения в переменные
  angle = myObject["an"];
  doorUp = myObject["Up"];
  doorDown = myObject["Dwn"];
  hydration_on = myObject["h_on"];
  watering_on_1 = myObject["w_1"];
  watering_on_2 = myObject["w_2"];
  nodeNumber = myObject["nd"];
  Serial.printf(msg.c_str(), "\n");
  }

 if (counter_1 > 0 && nodeNumber == 1 && counter_10 == 0){ // если шлюз не передает, берем команды от 1й платы
  //записываем значения в переменные
  jsonSend["nd"] = 1;
  angle = myObject["an"];
  jsonSend["an"] = angle;
  doorUp = myObject["Up"];
  jsonSend["Up"] = doorUp;
  doorDown = myObject["Dwn"];
  jsonSend["Dwn"] = doorDown;
  hydration_on = myObject["h_on"];
  jsonSend["h_on"] = hydration_on;
  watering_on_1 = myObject["w_1"];
  jsonSend["w_1"] = watering_on_1;
  watering_on_2 = myObject["w_2"];
  jsonSend["w_2"] = watering_on_2;
  String msg_to_r = JSON.stringify(jsonSend);
  Serial.printf(msg_to_r.c_str(), "\n");
  }

 if (counter_2 > 0 && nodeNumber == 2 && counter_10 == 0 && counter_1 == 0){
  // если шлюз не передает, и 1я не передает, берем команды от 2й платы
  //записываем значения в переменные
  jsonSend["nd"] = 2;
  angle = myObject["an"];
  jsonSend["an"] = angle;
  doorUp = myObject["Up"];
  jsonSend["Up"] = doorUp;
  doorDown = myObject["Dwn"];
  jsonSend["Dwn"] = doorDown;
  hydration_on = myObject["h_on"];
  jsonSend["h_on"] = hydration_on;
  watering_on_1 = myObject["w_1"];
  jsonSend["w_1"] = watering_on_1;
  watering_on_2 = myObject["w_2"];
  jsonSend["w_2"] = watering_on_2;
  String msg_to_r = JSON.stringify(jsonSend);
  Serial.printf(msg_to_r.c_str(), "\n");
  }
}

void newConnectionCallback(uint32_t nodeId) {
  //  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void countConnection(){
  // убывающий счетчик  сообщений от плат 10, 1 и 2
  if (counter_10 > 0) {
    counter_10--;
  }
   if (counter_1 > 0) {
    counter_1--;
  }
   if (counter_2 > 0) {
    counter_2--;
  }

/*  Serial.print("counter_10="); Serial.println(counter_10);
  Serial.print("counter_1="); Serial.println(counter_1);
  Serial.print("counter_2="); Serial.println(counter_2);*/
}

void blinker(){
// индикатор работы передачи команд
if (counter_10 > 0) {
  digitalWrite(BUILTIN_LED, !led); // выключаем led
}
  
}
