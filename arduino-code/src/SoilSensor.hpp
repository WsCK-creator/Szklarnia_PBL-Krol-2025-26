#pragma once

#include <Arduino.h>
#include "SoilSensorState.hpp"
#include <utility>

#define V_REF 500
#define ADC_STEPS 1023

using SoilCallback = void (*)(SoilSensorState);

class SoilSensor
{
private:
    //parameters
    unsigned char _enPin;
    unsigned char _sensorPin;
    unsigned char _hysteresis = 20;
    unsigned char _hysteresis_max = 40;
    unsigned char _hysteresis_min = 0;
    unsigned long _last_read = 0;
    unsigned short _read_delay = 2000; //ms
    SoilSensorState _soilState;
    SoilCallback _callback = nullptr;
    //functions
    void setEN_Pin(bool state);
    void setAndCall(SoilSensorState s);
   
public:
    //functions
    SoilSensor(unsigned char enPin, unsigned char sensorPin);
    void Init(SoilCallback valueChagedCallback);
    void setHysteresis(unsigned char hysteresis);
    unsigned char getHysteresis();
    std::pair<unsigned char, unsigned char> getMinMaxHysteresis();
    unsigned short getReadDelay();
    void setReadDelay(unsigned short delay);
    SoilSensorState getLastState();
    void readSensor();
};

inline void SoilSensor::setEN_Pin(bool state)
{
    if(digitalRead(_enPin) != state)
        digitalWrite(_enPin, state);
}

inline void SoilSensor::setAndCall(SoilSensorState s)
{
    _soilState = s;
    if(_callback) _callback(_soilState);
}

SoilSensor::SoilSensor(unsigned char enPin, unsigned char sensorPin)
{
    _enPin = enPin;
    _sensorPin = sensorPin;
}

inline void SoilSensor::Init(SoilCallback valueChagedCallback)
{
    _callback = valueChagedCallback;
    pinMode(_enPin, OUTPUT);
    setEN_Pin(false);
}

inline void SoilSensor::setHysteresis(unsigned char hysteresis)
{
    if(hysteresis <= 100)
        _hysteresis = hysteresis;
    else
        _hysteresis = 100;
}

inline unsigned char SoilSensor::getHysteresis() { return _hysteresis; }

inline std::pair<unsigned char, unsigned char> SoilSensor::getMinMaxHysteresis() { return std::make_pair(_hysteresis_min, _hysteresis_max); }

inline unsigned short SoilSensor::getReadDelay() { return _read_delay; }

inline void SoilSensor::setReadDelay(unsigned short delay){ _read_delay = delay; }

inline SoilSensorState SoilSensor::getLastState() { return _soilState; }

void SoilSensor::readSensor()
{
    if(millis() >  _last_read + _read_delay)
    {
        unsigned char value = (analogRead(_sensorPin) / ADC_STEPS) * V_REF;

        switch (_soilState)
        {
        case SOIL_WET:
            if (value >= SOIL_DRY + _hysteresis)
                setAndCall(SOIL_DRY);
            else if (value >= SOIL_MOIST +_hysteresis)
                setAndCall(SOIL_MOIST);
            break;
        case SOIL_MOIST:
            if(value <= SOIL_WET - _hysteresis)
                setAndCall(SOIL_WET);
            else if (value >= SOIL_DRY + _hysteresis)
                setAndCall(SOIL_DRY);
            break;
        case SOIL_DRY:
            if(value <= SOIL_WET - _hysteresis)
                setAndCall(SOIL_WET);
            else if (value <= SOIL_MOIST - _hysteresis)
                setAndCall(SOIL_MOIST);
            break;

        case UNINITIALIZED:
            if (value > SOIL_DRY - _hysteresis)
                setAndCall(SOIL_DRY);
            else if (value > SOIL_MOIST - _hysteresis)
                setAndCall(SOIL_MOIST);
            else
                setAndCall(SOIL_WET);
            break;
        }
        _last_read = millis();
    }
}

