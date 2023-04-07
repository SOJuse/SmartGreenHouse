//------------JSON------------

//получаем показания с датчиков
String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  water_lvl = readSensor();
  jsonReadings["waterLevel"] = water_lvl;

   // включение светодиода при малом объеме воды
  if (water_lvl < 100){
    digitalWrite(ledPin, HIGH);
  } else {
     digitalWrite(ledPin, LOW);
  }
  return JSON.stringify(jsonReadings);
}

//----------------------------

double readSensor()
{
  //  digitalWrite(sensorPower, HIGH);  // Включить датчик
  //  delay(10);                        // Дать время питанию установиться
  double val1 = analogRead(sensorPin);  // Прочитать аналоговое значение от датчика
  double val;
  //  digitalWrite(sensorPower, LOW);   // Выключить датчик
   val = val1;
   //
   (pow(2.7182818284, (-4.1810 + 0.0097 * val1)) / 3.6 * 100 - 0.42);
   return val;// Вернуть аналоговое значение влажности
}
