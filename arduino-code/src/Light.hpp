#pragma once

#include <ArduinoSTL.h>

#include "DataTypes.hpp"

#define V_REF 500
#define ADC_STEPS 1023

using LightCallback = void (*)(const void*, const unsigned char*);

class Light
{
private:
    const ConfigUShort _delay_cf = {wrapperReadDelay, 100, 60000, 50, "ms"};

    unsigned char _ph_pin;
    unsigned char _r_pin;
    unsigned char _g_pin;
    unsigned char _b_pin;

    unsigned char _light_level = 100;
    unsigned short _read_delay = 2000; //ms
    unsigned long _last_read = 0;
    bool _is_r_on = false;
    bool _is_g_on = false;
    bool _is_b_on = false;

    std::vector<std::pair<const void*, LightCallback>> _callbacks;
public:
    const DataConfig delayConfig = {TYPE_USHORT, {.confUShort = &_delay_cf}};

    Light(unsigned char phPin, unsigned char rLEDPin, unsigned char gLEDPin, unsigned char bLEDPin);
    void Init();
    void update();
    void addCallback(const void *context, LightCallback valueChagedCallback);

    unsigned short readDelay(const unsigned short* delay);
    bool rLED(const bool* state);
    bool gLED(const bool* state);
    bool bLED(const bool* state);
    unsigned char lightLevel();
    static unsigned short wrapperReadDelay(const void* context, const unsigned short* delay);
};

Light::Light(unsigned char phPin, unsigned char rLEDPin, unsigned char gLEDPin, unsigned char bLEDPin)
{
    _ph_pin = phPin;
    _r_pin = rLEDPin;
    _g_pin = gLEDPin;
    _b_pin = bLEDPin;
}

void Light::Init()
{
    pinMode(_r_pin, OUTPUT);
    pinMode(_g_pin, OUTPUT);
    pinMode(_b_pin, OUTPUT);
    digitalWrite(_r_pin, LOW);
    digitalWrite(_g_pin, LOW);
    digitalWrite(_b_pin, LOW);
    Serial.println("Light sensor initialized");
}

void Light::update()
{
    if(millis() - _last_read >= _read_delay)
    {
        unsigned short value = (analogRead(_ph_pin) / ADC_STEPS) * V_REF;
        unsigned char val = (value * 100) / V_REF;
        if(val != _light_level){
            _light_level = val;
            for (const auto& callback : _callbacks)
            {
                callback.second(callback.first, &_light_level);
            }
            Serial.print("Light level changed to: ");
            Serial.println(_light_level);
        }
        _last_read = millis();
    }
}

void Light::addCallback(const void *context, LightCallback valueChagedCallback)
{
    _callbacks.push_back(std::make_pair(context, valueChagedCallback));
}

unsigned short Light::readDelay(const unsigned short *delay)
{
    if(delay) _read_delay = *delay;
    return _read_delay;
}

bool Light::rLED(const bool *state)
{
    if(state && _is_r_on != *state)
    {
        _is_r_on = *state;
        digitalWrite(_r_pin, _is_r_on);
    }
    return _is_r_on;
}

bool Light::gLED(const bool *state)
{
    if(state && _is_g_on != *state)
    {
        _is_g_on = *state;
        digitalWrite(_g_pin, _is_g_on);
    }
    return _is_g_on;
}

bool Light::bLED(const bool *state)
{
    if(state && _is_b_on != *state)
    {
        _is_b_on = *state;
        digitalWrite(_b_pin, _is_b_on);
    }
    return _is_b_on;
}

unsigned char Light::lightLevel() { return _light_level; }

unsigned short Light::wrapperReadDelay(const void *context, const unsigned short *delay)
{
    Light* obj = (Light*)context;
    return obj->readDelay(delay);
}
