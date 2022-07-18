#include "TFT_eSPI.h" 
#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>

SensirionI2CSht4x sht4x;
TFT_eSPI tft; 
TFT_eSprite spr = TFT_eSprite(&tft);

void setup(void) {
  Wire.begin();
  sht4x.begin(Wire);
  tft.begin(); 
  tft.setRotation(3); 
  spr.createSprite(TFT_HEIGHT,TFT_WIDTH);
}

void loop() {
  float temperature, humidity;
  sht4x.measureHighPrecision(temperature, humidity);

  //Setting the title header 
  spr.fillSprite(TFT_WHITE); 
  spr.fillRect(0,0,320,50,TFT_DARKGREEN);  
  spr.setTextColor(TFT_WHITE); 
  spr.setTextSize(3); 
  spr.drawString("WEATHER STATION",30,15); 

  //Drawing horizontal line
  spr.drawFastHLine(0,140,320,TFT_DARKGREEN);

  //Setting temperature
  spr.setTextColor(TFT_BLACK);
  spr.setTextSize(3);
  spr.drawString("Temperature:",10,85);
  spr.setTextSize(3);
  spr.drawNumber(temperature,230,85); 
  spr.drawString("C",270,85);

  //Setting humidity
  spr.setTextSize(3);
  spr.drawString("Humidity:",10,180);
  spr.setTextSize(3);
  spr.drawNumber(humidity,180,180);
  spr.drawString("%RH",220,180);

  spr.pushSprite(0,0);
  delay(50);
}
