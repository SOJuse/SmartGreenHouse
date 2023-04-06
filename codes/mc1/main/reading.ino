//------------JSON------------

//получаем показания с датчиков
String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  #if (NOSENSORS == 1) // режим отладки без датчиков
   jsonReadings["temp"] = 24.35;
   jsonReadings["hum"] = 45;
   jsonReadings["ghum"] = readSensor();
   jsonReadings["nd"] = 1;
   jsonReadings["an"] = 30;
   jsonReadings["Up"] = 0;
   jsonReadings["Dwn"] = 1;
   jsonReadings["h_on"] = 0;
   jsonReadings["w_1"] = 0;
   jsonReadings["w_2"] = 0;
  #else
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  jsonReadings["temp"] = temp.temperature;
  jsonReadings["hum"] = humidity.relative_humidity;
  jsonReadings["ghum"] = readSensor();
  jsonReadings["nd"] = nodeNumber;
  jsonReadings["an"] = angle;
  jsonReadings["Up"] = doorUp;
  jsonReadings["Dwn"] = doorDown;
  jsonReadings["h_on"] = hydration_on;
  jsonReadings["w_1"] = watering_on_1;
  jsonReadings["w_2"] = watering_on_2;
  #endif
  return JSON.stringify(jsonReadings);
}

//----------------------------

int readSensor() 
{
//  digitalWrite(sensorPower, HIGH);  // Включить датчик
//  delay(10);                        // Дать время питанию установиться
  int val = analogRead(sensorPin);  // Прочитать аналоговое значение от датчика
//  digitalWrite(sensorPower, LOW);   // Выключить датчик
  int ans;
  Serial.print("Ground DAC=");
  Serial.println(val);
  if (val>850) ans=0;
  if (val>400 && val<850) ans=1;
  if (val<400) ans=2;
  return ans;                       // Вернуть аналоговое значение влажности
}
