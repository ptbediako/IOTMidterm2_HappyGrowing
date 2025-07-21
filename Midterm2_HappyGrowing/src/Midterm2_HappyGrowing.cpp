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

TCPClient TheClient;
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 
Adafruit_MQTT_Publish pubFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Midterm2_EnvrDashboard");
float pubValue;
void MQTT_connect();
bool MQTT_ping();

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
bool tempGood, humidGood, soilGood;

//OLED Variables
const int OLEDSCRN=0x3c;
const int OLED_RESET = -1;
Adafruit_SSD1306 display(OLED_RESET);
int displayMode;
int lastDisplayTime;

//Timestamp variables
String dateTime;
String timeHHMM; //formatted time var
String dateMMDD; //formatted date var
const int MDTTIME = -6;
const int MSTTIME = -7;
unsigned int lastTime;

//Neopixel variables
const int PIXELCOUNT=12;
int pix;
int startPixel, endPixel, color;
Adafruit_NeoPixel pixel (PIXELCOUNT,SPI1,WS2812B);
void pixelFill(int startPixel, int endPixel, int color);

struct SensorData {
  int airQuality;
  int soilDryness;
  float tempF;
  float humidRH;
  float pressInHg;
};
SensorData hgSensors;
void createEventPayLoad(SensorData hgSensors);

SYSTEM_MODE(SEMI_AUTOMATIC);

void displayMessage(int textSize, char message[]);


void setup() {
  Wire.begin();
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);
  
  
  Time.zone(MDTTIME);
  Particle.syncTime();

  status = bme.begin(BME280);
  if (status == false){
    Serial.printf("BME280 at address 0x%02x failed to start",BME280);
  }

  display.begin(SSD1306_SWITCHCAPVCC,OLEDSCRN);
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
}


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
  


  if(millis()-lastTime>4000){
    if(mqtt.Update()){
      createEventPayLoad(hgSensors);
    }
    
  
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.clearDisplay();
    display.printf("Hi! I'm a\nRED BANANA CROTON\n \nI like\nTemp: 65-85%cF\nHumidity: 50-70%cRH\nWater every 3-5 days",DEGREE,PCT);
    display.display();
    delay(500);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.clearDisplay();
    // display.printf("Screen2 Test Print");
    display.printf("Current Conditions\nTemp: %0.1f %cF\nHumidity: %0.1f %cRH\nPressure: %0.1f inHG\n",tempF, DEGREE, humidRH, PCT, pressInHg);
    display.display();
    delay(500);

     display.setTextSize(1);
     display.setTextColor(WHITE);
     display.setCursor(0,0);
     display.clearDisplay();
     display.printf("It is %s\n at %s\nSoil Dryness: %i\nAirQuality: %i",dateMMDD.c_str(), timeHHMM.c_str(), airQuality);
     display.display();
     delay(500);

    // if ((tempF<70)&&(tempF>71.5)){
    //   // display.setTextSize(2);
    //   // display.clearDisplay();
    //   // display.printf("I'm comfy");
    //   // display.display();
    //   //displayMessage(2,"I'm comfy");
    //   tempGood=1;
      
    Serial.printf("(Sensor value: %i)\n",airQuality);

    if (airQuality == AirQualitySensor::FORCE_SIGNAL){
      Serial.printf("Warning! Excessive pollution! ");
      displayMessage(1,"Warning!\nExcessive\npollution!");
    }
    else if (airQuality == AirQualitySensor::HIGH_POLLUTION) {
          Serial.printf("High pollution ");
          displayMessage(1,"High\npollution");
    } 
    else if (airQuality == AirQualitySensor::LOW_POLLUTION) {
        Serial.printf("Low pollution ");
        displayMessage(1,"Low\npollution");
    } 
    else if (airQuality == AirQualitySensor::FRESH_AIR) {
        displayMessage(1, "Fresh air");
    }
  
    if ((tempF>71.5)){     
      displayMessage(2,"I'm hot!");
      delay(500);
    }
    if ((tempF<70.5)){ 
      displayMessage(2,"I'm cold!");
      delay(500);
    }

    if ((humidRH <50)){
      displayMessage(2, "The air is\ntoo dry!");
      delay(500);
    }
    if ((humidRH >70)){
      displayMessage(2, "It's too\nhumid!");
      delay(500);
    }

    if((soilDryness>2500)){
      displayMessage(2, "The soil\nis too\ndry!");
         delay(500);
    }
    if((soilDryness<1700)){
      displayMessage(2, "I'm\ndrowning!");
         delay(500);
    }

    
    tempGood=1;
    humidGood=0;
    soilGood=1;
  
  if ((tempGood == 1) && (humidGood ==1) && (soilGood == 1)){
        startPixel=0;
        endPixel= 5;
        pixelFill(startPixel, endPixel, green);
        displayMessage(2, "I'm happy");
  }

  else{
    startPixel=6;
    endPixel= 11;
    pixelFill(startPixel, endPixel, red);
  }
 // }*/
 lastTime = millis ();
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

/************************************************************/
void createEventPayLoad(SensorData hgSensors){
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("Air Quality", airQuality);
    jw.insertKeyValue("Soil Dryness", soilDryness);
    jw.insertKeyValue("Temperature(F)", tempF);
    jw.insertKeyValue("Humidity", humidRH);
    jw.insertKeyValue("Air Pressure(Hg)", pressInHg);
  }
  pubFeed.publish(jw.getBuffer());
}