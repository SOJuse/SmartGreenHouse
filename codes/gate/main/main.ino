//шлюз

//#include <iocontrol.h>
#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include "painlessMesh.h"
#include <PubSubClient.h>

#define sensorPin A0 //датчик уровня
#define ledPin D13

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
#define   WIFI_CHANNEL    1

const char* mqtt_server = "dev.rightech.io";
const char* mqtt_username = "hihi23";
const char* mqtt_password = "hihi23";
const char* clientID = "mqtt-greenhouse1103";

byte autocmd = 0; //флаг автоматического управления

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
int angle = 42; //угол подъема по умолчанию
int set_hydration = 0; //уставка влажности по умолчанию
int set_temperature = 30; //уставка температуры по умолчанию
int set_gh = 1; //уставка влажности почвы по умолчанию
String water;
double temp = 15; //начальная температура, чтобы не было сработки
double temp1, temp2;
double hum = 50; //начальная влажность, чтобы не было сработки
double hum1, hum2;
byte ghum1 = 1; //начальная влажность почвы, чтобы не было сработки
byte ghum2 = 1;
byte doorUp, doorDown, hydration_on, watering_on_1, watering_on_2;
String s_ghum1, s_ghum2;
long int hum_time; //время, в которое запускается увлжанитель
long int hum_time_2; //время, в которое отключаем увлжанитель
bool hum_flag = 1; //флаг, отвечающий за активность увлажнителя
IPAddress getlocalIP();

IPAddress myIP(0, 0, 0, 0);
WiFiClient wifiClient;
PubSubClient mqttClient(mqtt_server, 1883, mqttCallback, wifiClient);


void setup() {
  pinMode(BUILTIN_LED, OUTPUT); // индикатор подключения к wifi
  digitalWrite(BUILTIN_LED, LOW); // включаем led до тех пор пока не подключимся к wifi
  Serial.begin(115200);

  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
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

  if (myObject.hasOwnProperty("set_temperature")) { // если передается уставка на температуру
    set_temperature = myObject["set_temperature"];
  }

  if (myObject.hasOwnProperty("watering_on_1")) { // если передается команда на полив первой помпы
    watering_on_1 = myObject["watering_on_1"];
  }

  if (myObject.hasOwnProperty("watering_on_2")) { // если передается команда на полив второй помпы
    watering_on_2 = myObject["watering_on_2"];
  }

  if (myObject.hasOwnProperty("set_gh")) { // если передается уставка на влажность почвы
    set_gh = myObject["set_gh"];
  }
  if (myObject.hasOwnProperty("auto")) { // если передается авторежим
    autocmd = myObject["auto"];
  }

}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}

void autoControl() {
 if (autocmd == 1){
  //управление влажностью
  if (hum < set_hydration and hydration_on == 0 and hum_flag == 1) {
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

  if (temp > set_temperature) {
    doorUp = 1;
    doorDown = 0;
  } 
  else {
    doorUp = 0;
    doorDown = 1;
  }

  //управление поливом
  if (ghum1 < set_gh) {
    watering_on_1 = 1;
  } else {
    watering_on_1 = 0;
  }

  if (ghum2 < set_gh) {
    watering_on_2 = 1;
  } else {
    watering_on_2 = 0;
  }
 }
}
