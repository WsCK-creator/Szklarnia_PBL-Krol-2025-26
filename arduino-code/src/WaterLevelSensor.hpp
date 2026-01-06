#pragma once

#include <ArduinoSTL.h>
#include <Wire.h>

#include "DataTypes.hpp"

#define NO_TOUCH       0xFE
#define THRESHOLD      100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR   0x77

using WaterLevelCallback = void (*)(const void*, const unsigned char*);


class WaterLevelSensor
{
private:
    const ConfigUShort _delay_cf = {wrapperReadDelay, 100, 60000, 50, "ms"};

    unsigned char _low_data[8] = {0};
    unsigned char _high_data[12] = {0};
    unsigned char _waterLevel = 0; //%
    unsigned long _last_read = 0;
    unsigned short _read_delay = 2000; //ms

    std::vector<std::pair<const void*, WaterLevelCallback>> _callbacks;

    void getHigh12SectionValue();
    void getLow8SectionValue();

public:
    const DataConfig delayConfig = {TYPE_USHORT, {.confUShort = &_delay_cf}};

    WaterLevelSensor();
    void readSensor();
    void Init();
    void addCallback(const void *context, WaterLevelCallback valueChagedCallback);

    unsigned short readDelay(const unsigned short* delay);

    unsigned char getWaterLevel();
    unsigned short getReadDelay();
    
    static unsigned short wrapperReadDelay(const void* context, const unsigned short* delay);
};

inline void WaterLevelSensor::getHigh12SectionValue()
{
    memset(_high_data, 0, sizeof(_high_data));
    Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
    while (12 != Wire.available());

    for (unsigned char i = 0; i < 12; i++) _high_data[i] = Wire.read();
    
}

inline void WaterLevelSensor::getLow8SectionValue()
{
    memset(_low_data, 0, sizeof(_low_data));
    Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
    while (8 != Wire.available());

    for (unsigned char i = 0; i < 8 ; i++) _low_data[i] = Wire.read();
}

WaterLevelSensor::WaterLevelSensor() {}

void WaterLevelSensor::readSensor()
{
    if(millis() - _last_read >= _read_delay + 10000)
    {
        Serial.println(_waterLevel);
        unsigned int touch_val = 0;
        unsigned char trig_section = 0;
        getLow8SectionValue();
        getHigh12SectionValue();

        for (unsigned char i = 0 ; i < 8; i++) {
            if (_low_data[i] > THRESHOLD) {
                touch_val |= 1 << i;

            }
        }
        for (unsigned char i = 0 ; i < 12; i++) {
            if (_high_data[i] > THRESHOLD) {
                touch_val |= (uint32_t)1 << (8 + i);
            }
        }

        while (touch_val & 0x01)
        {
            trig_section++;
            touch_val >>= 1;
        }
        if(trig_section * 5 != _waterLevel)
        {
            _waterLevel = trig_section * 5;
            for (const auto& callback : _callbacks)
            {
                callback.second(callback.first, &_waterLevel);
            }
            Serial.print("Water level changed to: ");
            Serial.println(_waterLevel);
        }
        _last_read = millis();
    }
}

inline void WaterLevelSensor::Init() 
{
    Wire.begin();
    Serial.print("Water level sensor initialized");
}

void WaterLevelSensor::addCallback(const void *context, WaterLevelCallback valueChagedCallback)
{
    _callbacks.push_back(std::make_pair(context, valueChagedCallback));
}

inline unsigned short WaterLevelSensor::readDelay(const unsigned short *delay)
{
    if(delay) _read_delay = *delay;
    return _read_delay;
}

inline unsigned char WaterLevelSensor::getWaterLevel() { return _waterLevel; }

inline unsigned short WaterLevelSensor::getReadDelay() { return _read_delay; }

unsigned short WaterLevelSensor::wrapperReadDelay(const void *context, const unsigned short *delay)
{
    WaterLevelSensor* obj = (WaterLevelSensor*)context;
    return obj->readDelay(delay);
}
