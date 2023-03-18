//запускается при подключение шлюза к серверу
void onConnectionEstablished() {
  Serial.println("new connected");
}

void publishData() {

// публикация данных на сервер

if (mqttClient.connected()) {
     mqttClient.publish("base/state/temperature", String(temp,2).c_str() );
     mqttClient.publish("base/state/temperature1", String(temp1,2).c_str() ); 
     mqttClient.publish("base/state/temperature2", String(temp2,2).c_str() );
     mqttClient.publish("base/state/waterlevel", String(water,2).c_str() );  
     Serial.println("Message to server sent");
        
}
  else{
   Serial.println("Can't publish. No connection to mqtt server");
      }
}


void serialDataSend () {
  JSONVar jsonReadings;
  
  jsonReadings["node"] = mynodeNumber;
  jsonReadings["angle"] = angle;
  jsonReadings["doorUp"] = doorUp;
  jsonReadings["doorDown"] = doorDown;
  String msg = JSON.stringify(jsonReadings);
  Serial.print("Send serialData =" );
  Serial.printf(msg.c_str(), "\n");
  Serial.println("");
  mesh.sendBroadcast( msg );
   
  
}
