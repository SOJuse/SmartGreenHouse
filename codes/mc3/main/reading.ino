//------------JSON------------

//получаем показания с датчиков
String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["waterLevel"] = readSensor();
  return JSON.stringify(jsonReadings);
 // jsonReadings["water-lvl"] = readSensor();
}

//----------------------------

double readSensor()
{
  //  digitalWrite(sensorPower, HIGH);  // Включить датчик
  //  delay(10);                        // Дать время питанию установиться
  double val = analogRead(sensorPin);  // Прочитать аналоговое значение от датчика
  //  digitalWrite(sensorPower, LOW);   // Выключить датчик
 // val = (pow(2.7182818284, (-4.1810 + 0.0097 * val)) / 3.6 * 100 - 0.42)/9.5/3.3*16.5*10.0;
  val = 8;
  Serial.println(val);
  return val;// Вернуть аналоговое значение влажности
}
