#include <Arduino.h>
#include "Enkoder.hpp"

#define ENCODER_CLK_PIN 3
#define ENCODER_DT_PIN 34
#define ENCODER_SW_PIN 2
#define DTH11_IN_PIN 32
#define DTH11_OUT_PIN 33
#define SOIL_SENSOR_1_PIN A0
#define SOIL_SENSOR_2_PIN A1
#define SOIL_SENSOR_3_PIN A2
#define LED_R_PIN 41
#define LED_G_PIN 39
#define LED_B_PIN 40
#define RELAY_ACTUATOR_PIN 46
#define RELAY_DIRECTION_PIN 45
#define RELAY_PUMP_PIN 44
#define RELAY_LED_PIN 43
#define RELAY_HEATER_PIN 42



Enkoder enkoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_SW_PIN);

void setup() {
  enkoder.Init(encBtPressed, encTurned);//add callback functions
}

void loop() {
  enkoder.loop();
}

void encBtPressed() {
  // Handle button press
}
void encTurned(int position) {
  // Handle encoder turn
}
