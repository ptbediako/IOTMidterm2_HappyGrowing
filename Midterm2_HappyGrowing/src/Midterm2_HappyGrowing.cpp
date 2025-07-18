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
#include "Button.h"
#include "Colors.h"
#include "IoTTimer.h"
#include "neopixel.h"

//Soil Moisture Sensor Variables
int soilMoistVal;

//SEEED Dust Sensor Variables

//BME Variables
float tempC, tempF, humidRH, pressPA, pressInHg;
const char DEGREE=0XF8;
const int BME280=0x76;
bool status;
Adafruit_BME280 bme;
bool tempGood, humidGood;

//OLED Variables
const int OLEDSCRN=0x3c;
const int OLED_RESET = -1;
Adafruit_SSD1306 display(OLED_RESET);

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

SYSTEM_MODE(AUTOMATIC);

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
}


void loop() {
  dateTime= Time.timeStr(); //current date and time
  timeHHMM = dateTime.substring(11,16); //for time that displays only hours and mins (no seconds)
  dateMMDD= dateTime.substring(4,9); //for date that displays only month and day (no day of week or year)
//  soilMoistVal = analogRead(A0);

  startPixel=0;
  endPixel= 5;
  pixelFill(startPixel, endPixel, green);

  startPixel=6;
  endPixel= 11;
  pixelFill(startPixel, endPixel, red);

  tempC=bme.readTemperature();
  tempF=(tempC*1.8)+32;
  humidRH=bme.readHumidity();
  pressPA= bme.readPressure();
  pressInHg=(pressPA * 0.0002952998751);
  
  //if(millis()-lastTime>4000){
    //lastTime = millis ();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.clearDisplay();
    display.printf("Hi! I'm a\nRED BANANA CROTON\n \nI like\nTemp: 65-85%cF\nHumidity: 50-70 RH\nWater every 3-5 days.",DEGREE);
    display.display();
    delay(2000);

    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // display.setCursor(0,0);
    // display.clearDisplay();
    // display.printf("It is %s\n at %s\nSoil Dryness: %i\nDust Concentration: %i",dateMMDD.c_str(), timeHHMM.c_str(), soilMoistVal);
    // //Serial.printf("Soil Moisture on %s at %s: %i\n",dateMMDD.c_str(), timeHHMM.c_str(), soilMoistVal);
    // display.display();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.clearDisplay();
    display.printf("Current Conditions\nTemp: %0.1f %cF\nHumidity: %0.1f RH\nPressure: %0.1f inHG\n",tempF, DEGREE, humidRH,pressInHg);
    display.display();
    delay(2000);

    //if ((tempF=73)){
      // display.setTextSize(2);
      // display.clearDisplay();
      // display.printf("I'm comfy");
      // display.display();
      //displayMessage(2,"I'm comfy");
    //  tempGood=1;
    //}
  
    if ((tempF>73)){
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.clearDisplay();
      display.printf("I'm hot!");
      display.display();
      delay(2000);
      //displayMessage(3,"I'm hot!");
    }

    if ((tempF<73)){
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.clearDisplay();
      display.printf("I'm cold!");
      display.display();
      delay(2000);
      //displayMessage(3,"I'm cold!");
    }
 // }

}

/*
void displayMessage(int textSize, char message[]){
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.printf(message);
  display.display();
}
*/
void pixelFill(int startPixel, int endPixel, int color){
  for(pix=startPixel; pix<=endPixel; pix=pix+1){
    pixel.setPixelColor(pix, color);
  }
  pixel.show();
  delay(100);
}
