void receivedCallback( uint32_t from, String &msg ) {
  
  JSONVar myObject = JSON.parse(msg.c_str());   //парсим полученные данные
  //записываем значения в переменные
  nodeNumber = myObject["node"];
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
  }
  temp = (temp1 + temp2) / 2;
  hum = (hum1 + hum2) / 2;
  Serial.print("MK1 ground =");
  Serial.println(s_ghum1);
  Serial.print("MK2 ground =");
  Serial.println(s_ghum2);
  Serial.print("MK3 water level =");
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

}
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection at gate, nodeId = %u\n", nodeId);
  Serial.printf("--> Start: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}
