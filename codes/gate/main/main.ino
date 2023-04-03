//шлюз

//#include <iocontrol.h>
#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include "painlessMesh.h"
#include <PubSubClient.h>


//--------PAINLESSMESH--------
#define   MESH_PREFIX     "teplitsa"   //логин  сети
#define   MESH_PASSWORD   "teplitsa"   //пароль
#define   MESH_PORT       5555   //порт 
#define   STATION_SSID "iPhone (Grisha)"
//#define   STATION_SSID "GDR"
#define   STATION_PASSWORD "12345678"
//#define   STATION_PASSWORD "chika16!"
#define   STATION_PORT     5555
#define   HOSTNAME         "MQTT_Bridge"
#define   WIFI_CHANNEL    6

const char* mqtt_server = "dev.rightech.io";
const char* mqtt_username = "hihi23";
const char* mqtt_password = "hihi23";
const char* clientID = "mqtt-greenhouse1103";


Scheduler userScheduler;   // планировщик
painlessMesh  mesh;   //обозначаем нашу библиотеку как mesh (для удобства)
void publishData() ;   //задаем прототип для коректной работы task
void mqttCallback(char* topic, byte* payload, unsigned int length); // прототип для mqttCallback
Task taskpublishData( TASK_SECOND * 15 , TASK_FOREVER, &publishData );   //указываем задание
void serialDataSend() ;   //задаем прототип для корректной работы task
Task taskSerialData( TASK_SECOND * 5 , TASK_FOREVER, &serialDataSend );   //указываем задание
void autoControl() ;   //задаем прототип для автоматического управления
Task taskAutoControl( TASK_SECOND * 5 , TASK_FOREVER, &autoControl );   //указываем задание
int nodeNumber;
byte mynodeNumber = 10; //указываем номер узла для шлюза
int angle = 30; //угол подъема по умолчанию
int set_hydration = 30; //уставка влажности по умолчанию
int set_temperature = 30; //уставка температуры по умолчанию
double water;
double temp = 15; //начальная температура, чтобы не было сработки
double temp1, temp2;
double hum = 50; //начальная влажность, чтобы не было сработки
double hum1, hum2;
byte ghum1, ghum2, doorUp, doorDown, hydration_on, watering_on_1, watering_on_2;
String s_ghum1, s_ghum2;
IPAddress getlocalIP();

IPAddress myIP(0, 0, 0, 0);
WiFiClient wifiClient;
PubSubClient mqttClient(mqtt_server, 1883, mqttCallback, wifiClient);


void setup() {
  pinMode(BUILTIN_LED, OUTPUT); // индикатор подключения к wifi
  digitalWrite(BUILTIN_LED, LOW); // включаем led до тех пор пока не подключимся к wifi
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

  userScheduler.addTask(taskpublishData);   //добавляем задание публикации на сервере в обработчик
  taskpublishData.enable();   //включаем задание
  userScheduler.addTask(taskSerialData);   //добавляем задание предачи данных на плату с исп.мех. в обработчик
  taskSerialData.enable();   //включаем задание
  userScheduler.addTask(taskAutoControl);   //добавляем задание автоконтроля
  taskAutoControl.enable();   //включаем задание
}

void loop() {
  mesh.update(); //для коректной работы mesha
  mqttClient.loop();

  if (myIP != getlocalIP()) {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
    //после получения ip адреса соединимся с сервером
    if (mqttClient.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT Broker");
      digitalWrite(BUILTIN_LED, HIGH); // отключили led
    }
    else {
      Serial.println("Connection to MQTT Broker failed...");
    }
  }

  /*   if (mqttClient.connected() == false) { // если соединение разорвалось, попробуем еще раз
         if (mqttClient.connect(clientID, mqtt_username, mqtt_password)) {
          Serial.println("Connected to MQTT Broker");
          }
          else {
           Serial.println("Connection to MQTT Broker failed...");
           }
       } */
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length + 1);
  memcpy(cleanPayload, payload, length);
  cleanPayload[length] = '\0';
  String msg = String(cleanPayload);
  free(cleanPayload);

  Serial.print("Message from mqtt=");
  Serial.println(msg);

  JSONVar myObject = JSON.parse(msg.c_str());   //парсим полученные данные

  if (myObject.hasOwnProperty("angle")) { // если передается угол
    angle = myObject["angle"];
  }

  if (myObject.hasOwnProperty("doorUp") || myObject.hasOwnProperty("doorDown")) { // если команда на форточку
    doorUp = myObject["doorUp"];
    doorDown = myObject["doorDown"];
  }

  if (myObject.hasOwnProperty("hydration_on")) { // если передается команда на увлажнение
    hydration_on = myObject["hydration_on"];
  }

  if (myObject.hasOwnProperty("set_hydration")) { // если передается уставка на влажность
    set_hydration = myObject["set_hydration"];
  }

  if (myObject.hasOwnProperty("watering_on_1")) { // если передается команда на полив первой помпы
    watering_on_1 = myObject["watering_on_1"];
  }

  if (myObject.hasOwnProperty("watering_on_2")) { // если передается команда на полив второй помпы
    watering_on_2 = myObject["watering_on_2"];
  }

  IPAddress getlocalIP() {
    return IPAddress(mesh.getStationIP());
  }

  void autoControl() {
    //управление влажностью
    if (hum < set_hydration) {
      hydration_on = 1;
    }
    else {
      hydration_on = 0;
    }
    //управление форточкой
    if (temp > set_temperature) {
      doorUp = 1;
      doorDown = 0;
    }
    else {
      doorUp = 0;
      doorDown = 1;
    }
    //управление поливом
    if (ghum1 < 1) {
      watering_on_1 = 1;
    }
    else {
      watering_on_1 = 0;
    }

    if (ghum2 < 1) {
      watering_on_2 = 1;
    }
    else {
      watering_on_2 = 0;
    }
  }
