#pragma once

#include <Arduino.h>
#include "DHT.h"
#include <utility>

using DHTCallback = void (*)(float, float);


class DHTSensor
{
private:
    unsigned long _last_read = 0;
    unsigned short _read_delay = 2000; //ms
    float _temperature = 0.0;
    float _humidity = 0.0;

    DHT _dht;
    DHTCallback _callback = nullptr;
    
public:
    DHTSensor(unsigned char pin);
    void Init(DHTCallback valueChagedCallback);
    void readSensor();
    unsigned short getReadDelay();
    void setReadDelay(unsigned short delay);
    std::pair<float, float> getLastData();
};

DHTSensor::DHTSensor(unsigned char pin) : _dht(pin, DHT11) {}

inline void DHTSensor::Init(DHTCallback valueChagedCallback) 
{ 
    _callback = valueChagedCallback;
    _dht.begin(); 
}

inline void DHTSensor::readSensor()
{
    if(millis() > _last_read + _read_delay)
    {
        float value = _dht.readTemperature();
        bool check = false;
        if(_temperature != value) 
        {
            check = true;
            _temperature = value;
        }
        value = _dht.readHumidity();
        if(_humidity != value)
        {
            check = true;
            _humidity = value;
        }

        if(check && _callback) 
            _callback(_temperature, _humidity);
        _last_read = millis();
    }
}

inline unsigned short DHTSensor::getReadDelay() { return _read_delay; }

inline void DHTSensor::setReadDelay(unsigned short delay) { _read_delay = delay; }

inline std::pair<float, float> DHTSensor::getLastData() { return std::make_pair(_temperature, _humidity); }
