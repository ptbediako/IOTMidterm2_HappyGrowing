/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

const int WATERPIN=D16;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);


void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  digitalWrite(WATERPIN,LOW);
  pinMode(WATERPIN,OUTPUT);
}


void loop() {
  digitalWrite(WATERPIN,HIGH);
  delay(750);

  digitalWrite(WATERPIN,LOW);
  delay(10000);
}
