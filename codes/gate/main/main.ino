//шлюз

#include <iocontrol.h>
#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include "painlessMesh.h"
//#include "EspMQTTClient.h"
#include <PubSubClient.h>
//#include <WiFi.h>
//#include <WiFiClient.h>

//*********ПЕРЕМЕННЫЕ*********
volatile boolean F = true;
double water;

//--------PAINLESSMESH--------
#define   MESH_PREFIX     "teplitsa"   //логин нашей сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт 
#define   STATION_SSID "GDR"
#define   STATION_PASSWORD "chika16!"
#define   STATION_PORT     5555
#define   HOSTNAME         "MQTT_Bridge"
#define   WIFI_CHANNEL    8

const char* mqtt_server = "dev.rightech.io";
const char* mqtt_username = "mqtt-1103"; 
const char* mqtt_password = "teplitsa"; 
const char* clientID = "mqtt-pa62-a5vfip";


Scheduler userScheduler;   // планировщик
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
void publishData() ;   //задаем пустышку для коректной работы task
void mqttCallback(char* topic, byte* payload, unsigned int length); // прототип для mqttCallback
Task taskpublishData( TASK_SECOND * 20 , TASK_FOREVER, &publishData );   //указываем задание
void serialDataSend() ;   //задаем пустышку для коректной работы task
Task taskSerialData( TASK_SECOND * 5 , TASK_FOREVER, &serialDataSend );   //указываем задание
int nodeNumber;
byte mynodeNumber = 10; //указываем номер ардуинки
int angle; //угол подъема 
double temp, temp1, temp2;
double hum, hum1, hum2;
byte ghum1, ghum2, doorUp, doorDown;
String s_ghum1, s_ghum2;
IPAddress getlocalIP();

IPAddress myIP(0,0,0,0);
WiFiClient wifiClient;
PubSubClient mqttClient(mqtt_server, 1883, mqttCallback, wifiClient);


void setup() {
  Serial.begin(115200);
 //--------------MESH--------------

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // установите перед функцией init() чтобы выдавались приветственные сообщения
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, WIFI_CHANNEL );
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);

  //назначаем функциям свои события
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

  userScheduler.addTask(taskpublishData);   //добавляем задание публикации iocontrol в обработчик
  taskpublishData.enable();   //включаем задание
  userScheduler.addTask(taskSerialData);   //добавляем задание предачи данных на плату с исп.мех. в обработчик
  taskSerialData.enable();   //включаем задание

 }

void loop() {
  mesh.update(); //для коректной работы mesha
  mqttClient.loop();
  
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
    //после получения ip адреса соединимся с сервером
    if (mqttClient.connect(clientID, mqtt_username, mqtt_password)) {
        Serial.println("Connected to MQTT Broker");
    }
    else {
    Serial.println("Connection to MQTT Broker failed...");
  }
  }

   if (mqttClient.connected() == false) { // если соединение разорвалось, попробуем еще раз
       if (mqttClient.connect(clientID, mqtt_username, mqtt_password)) {
        Serial.println("Connected to MQTT Broker");
        }
        else {
         Serial.println("Connection to MQTT Broker failed...");
         }
     }
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length+1);
  memcpy(cleanPayload, payload, length);
  cleanPayload[length] = '\0';
  String msg = String(cleanPayload);
  free(cleanPayload);

  String targetStr = String(topic).substring(16);

  if(targetStr == "gateway")
  {
    if(msg == "getNodes")
    {
      auto nodes = mesh.getNodeList(true);
      String str;
      for (auto &&id : nodes)
        str += String(id) + String(" ");
      mqttClient.publish("painlessMesh/from/gateway", str.c_str());
    }
  }
  else if(targetStr == "broadcast") 
  {
    mesh.sendBroadcast(msg);
  }
  else
  {
    uint32_t target = strtoul(targetStr.c_str(), NULL, 10);
    if(mesh.isConnected(target))
    {
      mesh.sendSingle(target, msg);
    }
    else
    {
      mqttClient.publish("painlessMesh/from/gateway", "Client not connected!");
    }
  }
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}
