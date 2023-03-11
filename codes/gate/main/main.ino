//шлюз

#include <iocontrol.h>
#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include "painlessMesh.h"
//#include "EspMQTTClient.h"
#include <PubSubClient.h>
//#include <WiFi.h>

//*********ПЕРЕМЕННЫЕ*********
volatile boolean F = true;
double water;

//--------PAINLESSMESH--------
#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт по дефолту 5555
Scheduler userScheduler;   // планировщик
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
void publishData() ;   //задаем пустышку для коректной работы task
Task taskpublishData( TASK_SECOND * 20 , TASK_FOREVER, &publishData );   //указываем задание
void serialDataSend() ;   //задаем пустышку для коректной работы task
Task taskSerialData( TASK_SECOND * 5 , TASK_FOREVER, &serialDataSend );   //указываем задание
int nodeNumber = 10; //указываем номер ардуинки
int angle; //угол подъема 
double temp, temp1, temp2;
double hum, hum1, hum2;
byte ghum1, ghum2, doorUp, doorDown;
String s_ghum1, s_ghum2;


//------------WIFI------------

//const char* ssid = "iPhone (Grisha)";
//const char* wifi_password = "12345678";
//const char* ssid = "AndroidA";
//const char* wifi_password = "chromech";
const char* ssid = "GDR";
const char* wifi_password = "chika16!";

WiFiClient wifiClient;

//----------IOCONTROL----------

// Название панели на сайте iocontrol.ru
const char* myPanelName = "smart_greenhouse1103";
int panelStatus;

// Название переменных как на сайте iocontrol.ru
const char* VarName_Temperature = "Temperature";
const char* VarName_Humidity = "Humidity";
const char* VarName_Water_Level = "Water_Level";
const char* VarName_Angle_Door = "Angle_Door";
const char* VarName_Ground_Humidity_1 = "Ground_Humidity_1";
const char* VarName_Ground_Humidity_2 = "Ground_Humidity_2";
const char* VarName_Door_Up = "Door_Up";
const char* VarName_Door_Down = "Door_Down";

  
  WiFiClient client;   // Создаём объект клиента класса EthernetClient
  //передаем в конструктр название панели и клиента
  iocontrol mypanel(myPanelName, client);

void setup() {
  Serial.begin(115200);
 //--------------MESH--------------

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // установите перед функцией init() чтобы выдавались приветственные сообщения
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );

  //назначаем функциям свои события
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

    // Вызываем функцию первого запроса к сервису
    mypanel.begin();
    userScheduler.addTask(taskpublishData);   //добавляем задание публикации iocontrol в обработчик
    taskpublishData.enable();   //включаем задание
    userScheduler.addTask(taskSerialData);   //добавляем задание предачи данных на плату с исп.мех. в обработчик
    taskSerialData.enable();   //включаем задание
}

void loop() {
  mesh.update(); //для коректной работы mesha

  
}
