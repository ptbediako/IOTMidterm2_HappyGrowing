# IoT Midterm 2 Happy Growing Smart Plant Care System
### Authored by Phylicia Bediako
### Hackster: 
[https://www.hackster.io/projects/b4e59b/edit#attachments](https://www.hackster.io/ptbediako/hg-smart-plant-care-system-b4e59b)

## Happy Growing: Intro and Motivation
The Happy Growing Smart Plant Care System helps simplify and automate plant care with environmental and soil sensors as well as hands-off watering options. As someone who chronically struggles with plant care, I wanted to create a system that would help me better understand what my plants need. The Happy Growing device includes an OLED programmed with messages "from" my plant that clearly communicates its preferences. After listing my plant type and its ideal conditions, the OLED displays the current conditions followed by complaints when the temperature, humidity, or soil conditions aren't quite right.

## Main Components
* Particle Photon2
* Environmental sensor: BME280 for measuring temperature, humidity, air pressure
* Air quality sensor: SEEED Grove Air Quality Sensor provides readings on levels of volatile organic compounds in the air
* Soil sensor: Capacitive soil moisture sensor for measuring soil moisture/dryness
* Watering system: Water pump with octocoupled relay provides manual and automatic soil watering options
* Display: OLED for displaying temperature, humidity, air pressure, soil moisture, air quality
* Status lights: Neopixels for displaying whether room and soil conditions are within bounds
  
## Parts
* Flower pot (3D printed)
* Platform with flower pot holder and water resevoir (3D printed)
* Console case (3D printed)

## Equipment
* Bambu Lab X1 Carbon 3D Printer
* Bambu Lab A1 Mini 3D Printer
  
## Software
* C/C++ in Visual Studio Code
* SolidWorks
* Bambu Studio
  
## Features
* Dynamic OLED display message for communicating ideal conditions for specific plants as well as current conditions
* Plain-English indicator messages when your plant's needs are not being met
* Easy-to-see control panel lights showing when your plant might be unhappy

## How it works and what it does
The system relies on a BME280 and SEEED Grove Air Quality Sensor to measure environmental conditions as well as a capacitive soil moisture sensor to keep track of when the plant needs to be watered. The device includes an automatic mode that waters the plant when the soil reaches a minimum level of moisture and a manual but remote method for watering the plant via an Adafruit dashboard. To the right of the panel, a NeoPixel ring helps illuminate a happy and sad face to help you see if your plant is okay at a glance. The sad face shows when any of the environmental or soil conditions are off, while the happy face lights up when your plant is satisfied.
