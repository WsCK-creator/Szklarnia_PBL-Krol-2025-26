#pragma once

#include <ArduinoSTL.h>

#include "SoilSensorState.hpp"
#include "DataTypes.hpp"

#define V_REF 500.0
#define ADC_STEPS 1023.0

using SoilCallback = void (*)(const void *, const unsigned char *, const SoilSensorState *);

class SoilSensor
{
private:
    const ConfigUShort _delay_cf = {wrapperReadDelay, 100, 60000, 50, "ms"};
    const ConfigUChar _hysteresis_cf = {wrapperHysteresis, 0, 40, 1, "V/100"};
    // parameters
    unsigned char _id;
    unsigned char _enPin;
    unsigned char _sensorPin;
    unsigned char _hysteresis = 10;
    unsigned long _last_read = 0;
    unsigned short _read_delay = 2000; // ms
    SoilSensorState _soilState;
    std::vector<std::pair<const void *, SoilCallback>> _callbacks;
    // functions
    void setEN_Pin(bool state);
    void setAndCall(SoilSensorState s);

public:
    const DataConfig delayConfig = {TYPE_USHORT, {.confUShort = &_delay_cf}};
    const DataConfig hysteresisConfig = {TYPE_UCHAR, {.confUChar = &_hysteresis_cf}};

    // functions
    SoilSensor(unsigned char enPin, unsigned char sensorPin, unsigned char id);
    void addCallback(const void *context, SoilCallback valueChagedCallback);
    void Init();

    unsigned short readDelay(const unsigned short *delay);
    unsigned char hysteresis(const unsigned char *hys);

    SoilSensorState getLastState();
    void readSensor();

    static unsigned short wrapperReadDelay(const void *context, const unsigned short *delay);
    static unsigned char wrapperHysteresis(const void *context, const unsigned char *hys);
};

inline void SoilSensor::setEN_Pin(bool state)
{
    if (digitalRead(_enPin) != state)
        digitalWrite(_enPin, state);
}

inline void SoilSensor::setAndCall(SoilSensorState s)
{
    if(_soilState == s) return;
    _soilState = s;

    for (const auto &callback : _callbacks)
    {
        callback.second(callback.first, &_id, &_soilState);
    }
    Serial.print("Soil sensor ");
    Serial.print(_id);
    Serial.print(" state changed to ");
    Serial.println(_soilState);
}

SoilSensor::SoilSensor(unsigned char enPin, unsigned char sensorPin, unsigned char id)
{
    _enPin = enPin;
    _sensorPin = sensorPin;
    _id = id;
}

void SoilSensor::addCallback(const void *context, SoilCallback valueChagedCallback)
{
    _callbacks.push_back(std::make_pair(context, valueChagedCallback));
}

inline void SoilSensor::Init()
{
    pinMode(_enPin, OUTPUT);
    setEN_Pin(false);
    Serial.print("Soil sensor ");
    Serial.print(_id);
    Serial.println(" initialized");
}

inline unsigned short SoilSensor::readDelay(const unsigned short *delay)
{
    if (delay)
    {
        _read_delay = *delay;
        return 0;
    }
    return _read_delay;
}

inline unsigned char SoilSensor::hysteresis(const unsigned char *hys)
{
    if (hys)
    {
        _hysteresis = *hys;
        return 0;
    }
    return _hysteresis;
}

inline SoilSensorState SoilSensor::getLastState() { return _soilState; }

void SoilSensor::readSensor()
{
    if (millis() - _last_read >= _read_delay)
    {
        short value = ((float)analogRead(_sensorPin) / ADC_STEPS) * V_REF;
        /*Serial.print("Soil sensor ");
        Serial.print(_id);
        Serial.print(" read value: ");
        Serial.print(value);
        Serial.print(" pin: ");
        Serial.println(_sensorPin);*/

        switch (_soilState)
        {
        case SOIL_E_WET:
        {
            if (value >= SOIL_E_DRY + _hysteresis)
                setAndCall(SOIL_E_DRY);
            else if (value >= SOIL_DRY + _hysteresis)
                setAndCall(SOIL_DRY);
            else if (value >= SOIL_MOIST + _hysteresis)
                setAndCall(SOIL_MOIST);
            else if (value >= SOIL_WET + _hysteresis)
                setAndCall(SOIL_WET);
        }
        case SOIL_WET:
        {
            if (value >= SOIL_E_DRY + _hysteresis)
                setAndCall(SOIL_E_DRY);
            else if (value >= SOIL_DRY + _hysteresis)
                setAndCall(SOIL_DRY);
            else if (value >= SOIL_MOIST + _hysteresis)
                setAndCall(SOIL_MOIST);
            else if (value <= SOIL_E_WET - _hysteresis)
                setAndCall(SOIL_E_WET);
            break;
        }
        case SOIL_MOIST:
        {
            if (value <= SOIL_E_WET - _hysteresis)
                setAndCall(SOIL_E_WET);
            else if (value <= SOIL_WET - _hysteresis)
                setAndCall(SOIL_WET);
            else if (value >= SOIL_DRY + _hysteresis)
                setAndCall(SOIL_DRY);
            else if (value >= SOIL_E_DRY + _hysteresis)
                setAndCall(SOIL_E_DRY);
            break;
        }
        case SOIL_DRY:
        {
            if (value >= SOIL_E_DRY + _hysteresis)
                setAndCall(SOIL_E_DRY);
            else if (value <= SOIL_E_WET - _hysteresis)
                setAndCall(SOIL_E_WET);
            else if (value <= SOIL_WET - _hysteresis)
                setAndCall(SOIL_WET);
            else if (value <= SOIL_MOIST - _hysteresis)
                setAndCall(SOIL_MOIST);
            break;
        }
        case SOIL_E_DRY:
        {
            if (value <= SOIL_E_WET - _hysteresis)
                setAndCall(SOIL_E_WET);
            else if (value <= SOIL_WET - _hysteresis)
                setAndCall(SOIL_WET);
            else if (value <= SOIL_MOIST - _hysteresis)
                setAndCall(SOIL_MOIST);
            else if (value <= SOIL_DRY - _hysteresis)
                setAndCall(SOIL_DRY);
            break;
        }
        case UNINITIALIZED:
        {
            if (value > SOIL_E_DRY - _hysteresis)
                setAndCall(SOIL_E_DRY);
            else if (value > SOIL_DRY - _hysteresis)
                setAndCall(SOIL_DRY);
            else if (value > SOIL_MOIST - _hysteresis)
                setAndCall(SOIL_MOIST);
            else if (value > SOIL_WET - _hysteresis)
                setAndCall(SOIL_WET);
            else
                setAndCall(SOIL_E_WET);
            break;
        }
        }
        _last_read = millis();
    }
}

unsigned short SoilSensor::wrapperReadDelay(const void *context, const unsigned short *delay)
{
    SoilSensor *obj = (SoilSensor *)context;

    return obj->readDelay(delay);
}

inline unsigned char SoilSensor::wrapperHysteresis(const void *context, const unsigned char *hys)
{
    SoilSensor *obj = (SoilSensor *)context;

    return obj->hysteresis(hys);
}
