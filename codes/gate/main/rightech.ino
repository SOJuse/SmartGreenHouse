//запускается при подключение шлюза к серверу
void onConnectionEstablished() {
  Serial.println("new connected");
}

void publishData() {

  // публикация данных на сервер

  if (mqttClient.connected()) {
    mqttClient.publish("base/state/temperature", String(temp, 2).c_str() );
    mqttClient.publish("base/state/temperature1", String(temp1, 2).c_str() );
    mqttClient.publish("base/state/temperature2", String(temp2, 2).c_str() );
    mqttClient.publish("base/state/waterLevel", water.c_str() );
    mqttClient.publish("base/state/humidity1", String(hum1, 2).c_str() );
    mqttClient.publish("base/state/humidity2", String(hum2, 2).c_str() );
    mqttClient.publish("base/state/humidity", String(hum, 2).c_str() );
    mqttClient.publish("base/state/ground-humidity-1", s_ghum1.c_str() );
    mqttClient.publish("base/state/ground-humidity-2", s_ghum2.c_str() );
    mqttClient.publish("base/state/angle", String(angle, DEC).c_str() );
    Serial.println("Message to server sent");

  }
  else {
    Serial.println("Can't publish. No connection to mqtt server");
  }
}


void serialDataSend () {
  JSONVar jsonReadings;

  jsonReadings["nd"] = mynodeNumber;
  jsonReadings["an"] = angle;
  jsonReadings["Up"] = doorUp;
  jsonReadings["Dwn"] = doorDown;
  jsonReadings["h_on"] = hydration_on;
  jsonReadings["w_1"] = watering_on_1;
  jsonReadings["w_2"] = watering_on_2;

  String msg = JSON.stringify(jsonReadings);
  Serial.print("Send serialData =" );
  Serial.printf(msg.c_str(), "\n");
  Serial.println("");
  mesh.sendBroadcast( msg );


}
