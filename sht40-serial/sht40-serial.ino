#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>

SensirionI2CSht4x sht4x;
float temperature;
float humidity;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sht4x.begin(Wire);
}

void loop() { 
  sht4x.measureHighPrecision(temperature, humidity);
  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.print("\t");
  Serial.print("Humidity:");
  Serial.println(humidity);
}
