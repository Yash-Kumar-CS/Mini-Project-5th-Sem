#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include<DHT.h>
#include<ThingSpeak.h>
#include <ESP8266WiFi.h>
 
String apiKey = "60SKOW8RQ4IUOMDE";     //  Enter your Write API key from ThingSpeak
 

const char *ssid =  "2a";     // replace with your wifi ssid and wpa2 key
const char *pass =  "yash@123";
const char* server = "api.thingspeak.com";

#define SDA_PIN D6 /* Define the SDA pin here  */
#define SCL_PIN D7 /* Define the SCL Pin here */

const int dhtPin = 4; // DHT11 data pin
const int mq135Pin = A0; // MQ135 analog output pin

DHT dht(dhtPin, DHT11);
WiFiClient client;
LiquidCrystal_I2C lcd(0x27,16,2);  /* set the LCD address to 0x27 for a 16 chars and 2 line display */

void setup(){
 // Wire.begin(SDA_PIN, SCL_PIN);  /* Initialize I2C communication */
  lcd.begin(16, 2); // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Live Monitoring ");
  lcd.setCursor(0,1);
  lcd.print("Weather Station");

    Serial.begin(9600);
       delay(10);
        dht.begin();
 
        Serial.println("Connecting to ");
        Serial.println(ssid);
 
 
        WiFi.begin(ssid, pass);
 
       while (WiFi.status() != WL_CONNECTED) 
      {
             delay(500);
             Serial.print(".");
     }
       Serial.println("");
       Serial.println("WiFi connected");
  delay(3000);

}

void loop(){
  float t = dht.readTemperature();
  int h = dht.readHumidity();
  if(t>50.00)
        t=0;
  if(h>100)
        h=0;
  int aqi = analogRead(mq135Pin);       
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("AQI : ");
  lcd.print(aqi);
  lcd.print(" ppm ");
  lcd.setCursor(0,1);
    lcd.print(" Current AQI ");
  
  
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp : ");
  lcd.print(t);
  lcd.print(" C");
  lcd.setCursor(0,1);
  lcd.print("Humidity : ");
  lcd.print(h);
  lcd.print(" %");
  delay(2000);

   if (isnan(h) || isnan(t)) 
                  {
                      Serial.println("Failed to read from DHT sensor!");
                       return;
                  }
 
                          if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                       {  
                            
                              String postStr = apiKey;
                              postStr +="&field1=";
                              postStr += String(aqi);
                               postStr +="&field2=";
                              postStr += String(t);
                              postStr +="&field3=";
                              postStr += String(h);
                              postStr += "\r\n\r\n";
 
                              client.print("POST /update HTTP/1.1\n");
                              client.print("Host: api.thingspeak.com\n");
                              client.print("Connection: close\n");
                              client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                              client.print("Content-Type: application/x-www-form-urlencoded\n");
                              client.print("Content-Length: ");
                              client.print(postStr.length());
                              client.print("\n\n");
                              client.print(postStr);
 
                              Serial.print("Temperature: ");
                              Serial.print(t);
                              Serial.print(" degrees Celcius, Humidity: ");
                              Serial.print(h);
                              Serial.println("%. Send to Thingspeak.");
                         }
           client.stop();
 
           Serial.println("Waiting...");
}


