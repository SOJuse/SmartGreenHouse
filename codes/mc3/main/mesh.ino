void sendMessage() {
  String msg = getReadings();
  mesh.sendBroadcast( msg );
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf(msg.c_str(), "\n");
/*  JSONVar myObject = JSON.parse(msg.c_str());   //парсим полученные данные
  //записываем значения в переменные
  int node = myObject["node"];
  double waterLevel = myObject["waterLevel"];*/
}
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}
