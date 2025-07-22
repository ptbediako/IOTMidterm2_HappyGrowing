/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Air_Quality_Sensor.h" //Library name: Grove_Air_quality_Sensor
#include "Button.h"
#include "Colors.h"
#include "IoTTimer.h"
#include "neopixel.h"
#include "credentials.h"
#include "JsonParserGeneratorRK.h"
// #include "../lib/Adafruit_MQTT/src/Adafruit_MQTT.h"
// #include "../lib/Adafruit_MQTT/src/Adafruit_MQTT_SPARK.h"
// #include "../lib/Grove_Air_quality_Sensor/src/Air_Quality_Sensor.h"
// #include "../lib/Adafruit_BME280/src/Adafruit_BME280.h"
// #include "../lib/Adafruit_SSD1306/src/Adafruit_SSD1306.h"
// #include "../lib/neopixel/src/neopixel.h"

//Subscribe & Publish
TCPClient TheClient;
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);  
Adafruit_MQTT_Publish hgSoilFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HGSoilFeed");
Adafruit_MQTT_Publish hgAirFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HGAirQualFeed");
Adafruit_MQTT_Publish hgTempFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HGTempFeed");
Adafruit_MQTT_Publish hgHumidFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HGHumidityFeed");
Adafruit_MQTT_Publish hgPressFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HGAirPressureFeed");
//Adafruit_MQTT_Subscribe subFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buttononoff");
Adafruit_MQTT_Subscribe hgButtonSub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/hgbuttonsub");

float pubValue;
int hgButtonPress; //replaced subValue
void MQTT_connect();
bool MQTT_ping();
unsigned int lastPubTime;

//Soil Moisture Sensor Variables
int soilDryness;
const int SOILPIN=A0;

//SEEED Air Quality Sensor Variables
const int AIRQUALPIN=A1;
int airQuality;
AirQualitySensor sensor(AIRQUALPIN);

//BME Variables
float tempC, tempF, humidRH, pressPA, pressInHg;
const char DEGREE=0xF8;
const char PCT=0x25;
const int BME280=0x76;
bool status;
Adafruit_BME280 bme;
bool tempProblem, humidProblem, soilProblem;

//OLED Variables
const int OLEDSCRN=0x3c;
const int OLED_RESET = -1;
Adafruit_SSD1306 display(OLED_RESET);
int displayMode;
unsigned int lastDisplayTime;

//Neopixel variables
const int PIXELCOUNT=12;
int pix;
int startPixel, endPixel, color;
Adafruit_NeoPixel pixel (PIXELCOUNT,SPI1,WS2812B);
void pixelFill(int startPixel, int endPixel, int color);

//Water Pump
const int WATERPIN=D16;
int waterTimer, lastWaterTimer;

//Timestamp variables
String dateTime;
String timeHHMM; //formatted time var
String dateMMDD; //formatted date var
const int MDTTIME = -6;
const int MSTTIME = -7;
unsigned int lastTime;

SYSTEM_MODE(AUTOMATIC);

void displayMessage(int textSize, char message[]);


void setup() {
  Wire.begin();
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);
  
  WiFi.on();
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

  mqtt.subscribe(&hgButtonSub);
  
  Time.zone(MDTTIME);
  Particle.syncTime();

   status = bme.begin(BME280);
   if (status == false){
     Serial.printf("BME280 at address 0x%02x failed to start",BME280);
   }

   display.begin(SSD1306_SWITCHCAPVCC,OLEDSCRN);
   display.setTextColor(WHITE);
   display.display();
   display.clearDisplay();


   pixel.begin();
   pixel.setBrightness(35);
   pixel.show();

   Serial.printf("Waiting for sensor to intitialize...");
   delay(20000);

   if(sensor.init()){
     Serial.printf("Sensor ready.");
   }
   else{
     Serial.printf("Sensor ERROR");
   }

  pinMode(WATERPIN,OUTPUT);
}

/***************************************************************/
void loop() {
  MQTT_connect();
  MQTT_ping();

  dateTime= Time.timeStr(); //current date and time
  timeHHMM = dateTime.substring(11,16); //for time that displays only hours and mins (no seconds)
  dateMMDD= dateTime.substring(4,9); //for date that displays only month and day (no day of week or year)
  soilDryness = analogRead(SOILPIN);
  airQuality=sensor.slope();

   tempC=bme.readTemperature();
   tempF=(tempC*1.8)+32;
   humidRH=bme.readHumidity();
   pressPA= bme.readPressure();
   pressInHg=(pressPA * 0.0002952998751);

   if((soilDryness>=1700)&&(soilDryness<=2500)){
    soilProblem=0;
   }
    if((soilDryness>2500)){
      soilProblem=1;
      }
    if((soilDryness<1700)){
      soilProblem=1;
    }

  if((humidRH>=50)&&(humidRH<=70)){
    humidProblem=0;
  }
  if ((humidRH <50)){
    humidProblem=1;
  }
  if ((humidRH >70)){
    humidProblem=1;
  }

  if((tempF>=65) &&(tempF<=85)){
    tempProblem=0;
  }
  if ((tempF>85)){     
    tempProblem=1;
  }
  if ((tempF<65)){ 
    tempProblem=1;
  }
  Serial.printf("T %i, H %i, S %i", tempProblem, humidProblem, soilProblem);

  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &hgButtonSub) {
      hgButtonPress = atoi((char *)hgButtonSub.lastread);

       if(hgButtonPress==1){
        digitalWrite(WATERPIN,HIGH);
       }
       if(hgButtonPress==0){
        digitalWrite(WATERPIN,LOW);
       }
     }
      Serial.printf("HG Water Button %i\n",hgButtonPress);
  }

  if((millis()-lastPubTime)>30000){
    if(mqtt.Update()){
      hgSoilFeed.publish(soilDryness,1);
      hgAirFeed.publish(airQuality,1);
      hgTempFeed.publish(tempF,1);
      hgHumidFeed.publish(humidRH,1);
      hgPressFeed.publish(pressInHg,1);
    }
    lastPubTime = millis();
  }

  


//Displays
 if ((millis()-lastDisplayTime)>4000){
  displayMode++;
  if(displayMode==6){
    displayMode=1;
  }
   lastDisplayTime = millis();
 }

//Screen 1: Intro
  if ((displayMode==1)){
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.printf("Hi! I'm a\nRED BANANA CROTON\n \nI like\nTemp: 65-85%cF\nHumidity: 50-70%cRH\nWater every 3-5 days",DEGREE,PCT);
    display.display();
  }

//Screen 2: Current Conditions
  if((displayMode==2)){
    display.setTextSize(1);
    display.setCursor(0,0);
    display.clearDisplay();
    display.printf("%s at %s\nTemp: %0.1f %cF\nHumidity: %0.1f %cRH\nSoil Dryness: %i\nAirQuality: %i\nPressure: %0.1f inHG",dateMMDD.c_str(), timeHHMM.c_str(), tempF, DEGREE, humidRH, PCT, soilDryness,airQuality, pressInHg);
    display.display();
  }

//Screen 3: Temperature Problems
  if((displayMode==3)){ 
    if ((tempF>85)){     
      displayMessage(2,"I'm hot!");
    }
    if ((tempF<65)){ 
      displayMessage(2,"I'm cold!");
    }
  }

//Screen 4: Humidity Problems
  if((displayMode==4)){
    if ((humidRH <50)){
      displayMessage(2, "The air is\ntoo dry!");
    }
    if ((humidRH >70)){
      displayMessage(2, "It's too\nhumid!");
    }
  }

//Screen 5: Soil Watering Problems
  if((displayMode==5)){
    if((soilDryness>2500)){
      displayMessage(2, "The soil\nis too\ndry!");
      }

    if((soilDryness<1700)){
      displayMessage(2, "I'm\ndrowning!");
    }
  }  

  //Watering the Soil
  waterTimer=millis();

  if((soilDryness>2500)){
    if((waterTimer-lastWaterTimer)>60000){
      digitalWrite(WATERPIN,HIGH);
    }
    if((waterTimer-lastWaterTimer)>61000){
      lastWaterTimer=waterTimer;
      digitalWrite(WATERPIN,LOW);
    }
  }

//NeoPixels
  if ((tempProblem == 0) && (humidProblem ==0) && (soilProblem == 0)){
    startPixel=0;
    endPixel= 5;
    pixelFill(startPixel, endPixel, green);

    startPixel=6;
    endPixel= 11;
    pixelFill(startPixel, endPixel, 0x000000);
  }
  if ((tempProblem == 1) || (humidProblem ==1) || (soilProblem == 1)){
    startPixel=6;
    endPixel= 11;
    pixelFill(startPixel, endPixel, red);

    startPixel=0;
    endPixel= 5;
    pixelFill(startPixel, endPixel, 0x000000);
  }

  //Air Quality
  Serial.printf("(Sensor value: %i)\n",airQuality);
  if (airQuality == AirQualitySensor::FORCE_SIGNAL){
    Serial.printf("Warning! Excessive pollution!");
    }
  if (airQuality == AirQualitySensor::HIGH_POLLUTION) {
    Serial.printf("High pollution ");
  }
  if (airQuality == AirQualitySensor::LOW_POLLUTION) {
    Serial.printf("Low pollution ");
  }
  if (airQuality == AirQualitySensor::FRESH_AIR) {
    Serial.printf("Fresh air ");  
  }

}

/************************************************************/
void displayMessage(int textSize, char message[]){
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.printf(message);
  display.display();
}

/************************************************************/
void pixelFill(int startPixel, int endPixel, int color){
  for(pix=startPixel; pix<=endPixel; pix=pix+1){
    pixel.setPixelColor(pix, color);
  }
  pixel.show();
  delay(100);
}

/************************************************************/
void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

/************************************************************/
bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}
