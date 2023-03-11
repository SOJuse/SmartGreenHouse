//запускается при подключение шлюза к серверу
void onConnectionEstablished() {
  Serial.println("new connected");
}

void publishData() {
 disconnecting_mesh();
 //Запускаем WIFI
  WIFIinit();

// ************************ ЧТЕНИЕ ************************

    // Чтение значений переменных из сервиса
    panelStatus = mypanel.readUpdate();
    // Если статус равен константе OK...
    if (panelStatus == OK) {
        // Выводим текст в последовательный порт
        Serial.println("------- Read OK -------");
        // Записываем считанный из сервиса значения в переменные
        long io_Angle_Door = mypanel.readInt(VarName_Angle_Door);   // целочисленная переменная
        Serial.println((String)"io_Angle_Door = "+io_Angle_Door);
        long io_Door_Up = mypanel.readInt(VarName_Door_Up);   // целочисленная переменная
        Serial.println((String)"io_Door_Up = "+io_Door_Up);
        long io_Door_Down = mypanel.readInt(VarName_Door_Down);   // целочисленная переменная
        Serial.println((String)"io_Door_Down = "+io_Door_Down);
        angle = io_Angle_Door;
        doorUp = io_Door_Up;
        doorDown = io_Door_Down;
        delay(500);
        if (io_Door_Up == 1){
          Serial.println(angle);
          Serial.println ("Up");
          mypanel.write(VarName_Door_Up, 0); 
        }
        if (io_Door_Down == 1) {
           Serial.println(angle);
           Serial.println("Down");
           mypanel.write(VarName_Door_Down, 0); 
           }
    } 

    // ************************ ЗАПИСЬ ************************
    
    // Записываем  значение в переменную для отправки в сервис
    mypanel.write(VarName_Temperature, temp);   // Записали значение температуры
    mypanel.write(VarName_Humidity, hum);   // Записали значение влажности
    mypanel.write(VarName_Water_Level, water);   // Записали значение уровня воды
    mypanel.write(VarName_Ground_Humidity_1, s_ghum1);  // Записали влажность почвы 1
    mypanel.write(VarName_Ground_Humidity_2, s_ghum2);  // Записали влажность почвы 2
    
    // Отправляем переменные из контроллера в сервис
    panelStatus = mypanel.writeUpdate();
    // Если статус равен константе OK...
    if (panelStatus == OK) {
        // Выводим текст в последовательный порт
        Serial.println("------- Write OK -------");
    }

 WiFi.mode(WIFI_OFF);
//  Serial.println(WiFi.status());
  delay(500);
  
 connecting_mesh();
}

void serialDataSend () {
  JSONVar jsonReadings;
  if (F == true) 
  {
  jsonReadings["node"] = nodeNumber;
  jsonReadings["angle"] = angle;
  jsonReadings["doorUp"] = doorUp;
  jsonReadings["doorDown"] = doorDown;
  String msg = JSON.stringify(jsonReadings);
  mesh.sendBroadcast( msg );
  Serial.println("Send serialData");
  }
  else
  {
  Serial.println("Can't send serialData");
  }
  
}
