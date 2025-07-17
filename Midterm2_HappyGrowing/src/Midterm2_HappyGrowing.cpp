/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}


/*
// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

int soilMoistVal;

const int OLEDSCRN=0x3c;
const int OLED_RESET = -1;
Adafruit_SSD1306 display(OLED_RESET);

String dateTime;
String timeHHMM; //formatted time var
String dateMMDD; //formatted date var
const int MDTTIME = -6;
const int MSTTIME = -7;
unsigned int lastTime;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);


void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

  Time.zone(MDTTIME);
  Particle.syncTime();

  display.begin(SSD1306_SWITCHCAPVCC,OLEDSCRN);
  display.display();
  display.clearDisplay();

}


void loop() {
  dateTime= Time.timeStr(); //current date and time
  timeHHMM = dateTime.substring(11,16); //for time that displays only hours and mins (no seconds)
  dateMMDD= dateTime.substring(4,9); //for date that displays only month and day (no day of week or year)
  soilMoistVal = analogRead(A0);
  
  if(millis()-lastTime>4000){
    lastTime = millis ();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.clearDisplay();
    
    display.printf("Plant Conditions on\n%s at %s\nSoil Moisture: %i\n",dateMMDD.c_str(), timeHHMM.c_str(), soilMoistVal);
    Serial.printf("Soil Moisture on %s at %s: %i\n",dateMMDD.c_str(), timeHHMM.c_str(), soilMoistVal);
    display.display();
  }
 
}

*/