//------------JSON------------

//получаем показания с датчиков
String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  #if (NOSENSORS == 1) // режим отладки без датчиков
   jsonReadings["temp"] = 24.85;
   jsonReadings["hum"] = 51;
   jsonReadings["ghum"] = readSensor();
  #else
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  jsonReadings["temp"] = temp.temperature;
  jsonReadings["hum"] = humidity.relative_humidity;
  jsonReadings["ghum"] = readSensor();
  #endif
  return JSON.stringify(jsonReadings);
}

//----------------------------

int readSensor() 
{
 // digitalWrite(sensorPower, HIGH);  // Включить датчик
//  delay(10);                        // Дать время питанию установиться
  int val = analogRead(sensorPin);  // Прочитать аналоговое значение от датчика
 // digitalWrite(sensorPower, LOW);   // Выключить датчик
  int ans;
  Serial.print("Ground DAC=");
  Serial.println(val);
  if (val>850) ans=0;
  if (val>400 && val<850) ans=1;
  if (val<400) ans=2;
  return ans;                       // Вернуть аналоговое значение влажности
}
