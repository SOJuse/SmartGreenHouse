void sendMessage() {
  String msg = getReadings();
  mesh.sendBroadcast( msg );
  autoControl();
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.print("receive msg=");
  Serial.printf(msg.c_str(), "\n");
 /* JSONVar myObject = JSON.parse(msg.c_str());   //парсим полученные данные
  //записываем значения в переменные
  int node = myObject["node"];
  double temp = myObject["temp"];
  double hum = myObject["hum"];
  double ghum = myObject["ghum"];
  Serial.print("Node: ");
  Serial.println(node);
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("Ground:");
  Serial.print(ghum);*/
}
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}
