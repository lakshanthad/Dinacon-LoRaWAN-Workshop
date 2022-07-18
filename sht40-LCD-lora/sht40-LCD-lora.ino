#include "TFT_eSPI.h" 
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>

SoftwareSerial mySerial(A0, A1); // RX, TX

SensirionI2CSht4x sht4x;
TFT_eSPI tft; 
TFT_eSprite spr = TFT_eSprite(&tft);

float temperature, humidity;
int int_temp, int_humi;

static char recv_buf[512];
static bool is_exist = false;
static bool is_join = false;
 
static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch;
    int num = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    mySerial.printf(p_cmd, args);
    Serial.printf(p_cmd, args);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
    {
        return 0;
    }
 
    do
    {
        while (mySerial.available() > 0)
        {
            ch = mySerial.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }
 
        if (strstr(recv_buf, p_ack) != NULL)
        {
            return 1;
        }
 
    } while (millis() - startMillis < timeout_ms);
    return 0;
}
 
static void recv_prase(char *p_msg)
{
    if (p_msg == NULL)
    {
        return;
    }
    char *p_start = NULL;
    int data = 0;
    int rssi = 0;
    int snr = 0;
 
    p_start = strstr(p_msg, "RX");
    if (p_start && (1 == sscanf(p_start, "RX: \"%d\"\r\n", &data)))
    {
        Serial.println(data);
    }
 
    p_start = strstr(p_msg, "RSSI");
    if (p_start && (1 == sscanf(p_start, "RSSI %d,", &rssi)))
    {
        Serial.println(rssi);
    }
 
    p_start = strstr(p_msg, "SNR");
    if (p_start && (1 == sscanf(p_start, "SNR %d", &snr)))
    {
        Serial.println(snr);
    }
}

void setup(void) {
  Serial.begin(115200); //Start serial communication
  mySerial.begin(9600);
  
  Wire.begin();
  sht4x.begin(Wire);
  
  if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
  {
      is_exist = true;
      at_send_check_response("+ID: DevEui", 1000, "AT+ID=DevEui,\"2CF7FXXXXXX0A49F\"\r\n");
      at_send_check_response("+ID: AppEui", 1000, "AT+ID=AppEui,\"8000XXXXXX000006\"\r\n");
      at_send_check_response("+MODE: LWOTAA", 1000, "AT+MODE=LWOTAA\r\n");
      at_send_check_response("+DR: AS923", 1000, "AT+DR=AS923\r\n");
      at_send_check_response("+CH: NUM", 1000, "AT+CH=NUM,0-1\r\n");
      at_send_check_response("+KEY: APPKEY", 1000, "AT+KEY=APPKEY,\"2B7E151628XXXXXXXXXX158809CF4F3C\"\r\n");
      at_send_check_response("+CLASS: A", 1000, "AT+CLASS=A\r\n");
      at_send_check_response("+PORT: 8", 1000, "AT+PORT=8\r\n");
      delay(200);
      is_join = true;
  }
  else
  {
      is_exist = false;
      Serial.print("No E5 module found.\r\n");
  }
  
  tft.begin(); 
  tft.setRotation(3); 
  spr.createSprite(TFT_HEIGHT,TFT_WIDTH);
}

void loop() {
  sht4x.measureHighPrecision(temperature, humidity);
  int_temp = temperature*100;
  int_humi = humidity*100;

  if (is_exist){
    int ret = 0;
    if (is_join){
      ret = at_send_check_response("+JOIN: Network joined", 12000, "AT+JOIN\r\n");
      if (ret){     
        is_join = false;
      }
      else{
        Serial.println("");
        Serial.print("JOIN failed!\r\n\r\n");
        delay(5000);
      }
     }
     else{
      char cmd[128];
      sprintf(cmd, "AT+CMSGHEX=\"%08X %08X\"\r\n", int_temp, int_humi);
      ret = at_send_check_response("oe", 10000, cmd);
      if (ret){      
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("\t");
        Serial.print("Humidity:");
        Serial.println(humidity);
        recv_prase(recv_buf);
       }
       else{       
        Serial.print("Send failed!\r\n\r\n");
       }
       delay(10000);
       }
  }
  else{ 
    delay(1000);
  }

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
