#include <Arduino.h>
#include "Enkoder.hpp"
#include "DHT.h"
#include "SoilSensor.hpp"
#include "SoilSensorState.hpp"
#include "DHTSensor.hpp"
#include "virtuabotixRTC.h"
#include <avr/wdt.h>

#define ENCODER_CLK_PIN 3
#define ENCODER_DT_PIN 34
#define ENCODER_SW_PIN 2
#define DTH11_IN_PIN 32
#define DTH11_OUT_PIN 33
#define SOIL_SENSOR_1_PIN A0
#define SOIL_SENSOR_2_PIN A1
#define SOIL_SENSOR_3_PIN A2
#define SOIL_SENSOR_EN_PIN 35
#define LED_R_PIN 41
#define LED_G_PIN 39
#define LED_B_PIN 40
#define RELAY_ACTUATOR_PIN 46
#define RELAY_DIRECTION_PIN 45
#define RELAY_PUMP_PIN 44
#define RELAY_LED_PIN 43
#define RELAY_HEATER_PIN 42

#define RESET_TIME 2592000000


Enkoder enkoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_SW_PIN);
SoilSensor soilSensor1(SOIL_SENSOR_EN_PIN, SOIL_SENSOR_1_PIN);
SoilSensor soilSensor2(SOIL_SENSOR_EN_PIN, SOIL_SENSOR_2_PIN);
SoilSensor soilSensor3(SOIL_SENSOR_EN_PIN, SOIL_SENSOR_3_PIN);
DHTSensor dhtIn(DTH11_IN_PIN);
DHTSensor dhtOut(DTH11_OUT_PIN);


void encBtPressed() {
  // Handle button press
}
void encTurned(int position) {
  // Handle encoder turn
}
void soilSensor1Change(SoilSensorState){

}
void soilSensor2Change(SoilSensorState){

}
void soilSensor3Change(SoilSensorState){

}

void dhtInChange(float temp, float hum){

}
void dhtOutChange(float temp, float hum){
  
}

void checkTimeOverflow(){
  if (millis() > RESET_TIME)
  {
    wdt_enable(WDTO_15MS);
    while (true) {}
  }
}


void setup() {
  enkoder.Init(encBtPressed, encTurned);
  soilSensor1.Init(soilSensor1Change);
  soilSensor2.Init(soilSensor2Change);
  soilSensor3.Init(soilSensor3Change);
  dhtIn.Init(dhtInChange);
  dhtOut.Init(dhtOutChange);
}

void loop() {
  enkoder.loop();
  soilSensor1.readSensor();
  soilSensor2.readSensor();
  soilSensor3.readSensor();
  dhtIn.readSensor();
  dhtOut.readSensor();

  checkTimeOverflow();
}