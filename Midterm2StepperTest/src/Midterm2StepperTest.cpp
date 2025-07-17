/* 
 * Project Stepper Test
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Stepper.h"

const int SPR=2048; //steps per revolution, this stays at 2048
const int IN1=D7;
const int IN3=D5;
const int IN2=D6;
const int IN4=D4;
const int SPEED= 1; //speed can go up to 10-15rpm
const signed int STEPS=1024; //also based on the 2048, but can be any number. If it's higher than 2048, it'll be multiple revolutions- 2048 * n revolutions
const signed int REVSTEPS=-1024;

SYSTEM_MODE(SEMI_AUTOMATIC);

Stepper myStepper(SPR, IN1, IN3, IN2, IN4);

void setup() {
  myStepper.setSpeed(SPEED); 
}


void loop() {
  myStepper.step(STEPS);
  myStepper.step(REVSTEPS);
}
