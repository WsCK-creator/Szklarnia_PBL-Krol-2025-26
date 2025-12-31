#include <Arduino.h>
#include <avr/wdt.h>

#include "Enkoder.hpp"
#include "SoilSensor.hpp"
#include "DHTSensor.hpp"
#include "WaterLevelSensor.hpp"
#include "Relays.hpp"
#include "virtuabotixRTC.h"
#include "Light.hpp"
#include "Processor.hpp"
#include "Disp.hpp"

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
#define LIGHT_SENSOR_PIN A3
#define RTC_CLK 38
#define RTC_DAT 37
#define RTC_RST 36
#define OLED_MOSI 52
#define OLED_SCK 51
#define OLED_CS 49
#define OLED_DC 48
#define OLED_RES 47

#define RESET_TIME 2592000000


Enkoder enkoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_SW_PIN);
SoilSensor soilSensor1(SOIL_SENSOR_EN_PIN, SOIL_SENSOR_1_PIN, 1);
SoilSensor soilSensor2(SOIL_SENSOR_EN_PIN, SOIL_SENSOR_2_PIN, 2);
SoilSensor soilSensor3(SOIL_SENSOR_EN_PIN, SOIL_SENSOR_3_PIN, 3);
DHTSensor dhtIn(DTH11_IN_PIN);
DHTSensor dhtOut(DTH11_OUT_PIN);
WaterLevelSensor waterLevelSensor;
Relays relays;
virtuabotixRTC myRTC(RTC_CLK, RTC_DAT, RTC_RST);
Disp disp(OLED_CS, OLED_RES, OLED_DC);
Light light(LIGHT_SENSOR_PIN, LED_R_PIN, LED_G_PIN, LED_B_PIN);
Processor processor;

void checkTimeOverflow(){
  if (millis() > RESET_TIME)
  {
    wdt_enable(WDTO_15MS);
    while (true) {}
  }
}

void setup() {
  enkoder.Init();
  soilSensor1.Init();
  soilSensor2.Init();
  soilSensor3.Init();
  waterLevelSensor.Init();
  dhtIn.Init();
  dhtOut.Init();
  relays.Init();
  disp.Init(&dhtIn, &dhtOut, &soilSensor1, &soilSensor2, &soilSensor3, &waterLevelSensor, &light, &relays, &enkoder, &myRTC, &processor);
  light.Init();
  processor.Init(&dhtIn, &dhtOut, &soilSensor1, &soilSensor2, &soilSensor3, &waterLevelSensor, &light, &relays);
}

void loop() {
  enkoder.loop();
  soilSensor1.readSensor();
  soilSensor2.readSensor();
  soilSensor3.readSensor();
  waterLevelSensor.readSensor();
  dhtIn.readSensor();
  dhtOut.readSensor();
  relays.update();
  light.update();
  processor.update();
  disp.update();

  checkTimeOverflow(); //TODO: zmienić na wykorzystaniem myRTC
}