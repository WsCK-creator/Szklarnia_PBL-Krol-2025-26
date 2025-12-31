#pragma once

#include<ArduinoSTL.h>
#include<DHT.h>

#include "DataTypes.hpp"

using DHTCallback = void (*)(const void*, const float*, const float*);

class DHTSensor
{
private:
    const ConfigUShort delay_cf = {wrapperReadDelay, 100, 60000, 50, "ms"};

    unsigned long _last_read = 0;
    unsigned short _read_delay = 10000; //ms
    float _temperature = 0.0; 
    float _humidity = 0.0;
    
    DHT _dht;
    //DHTCallback _callback = nullptr;
    std::vector<std::pair<const void*, DHTCallback>> _callbacks;
    
public:
    const DataConfig delayConfig = {TYPE_USHORT, {.confUShort = &delay_cf}};

    DHTSensor(unsigned char pin);
    void addCallback(const void* context, DHTCallback valueChagedCallback);
    void Init();
    void readSensor();

    unsigned short readDelay(const unsigned short* delay);
    std::pair<float, float> getLastData();

    static unsigned short wrapperReadDelay(const void* context, const unsigned short* delay);
    //static std::pair<float, float> wrapperGetLastData()
};

DHTSensor::DHTSensor(unsigned char pin) : _dht(pin, DHT11) {}

inline void DHTSensor::addCallback(const void* context, DHTCallback valueChagedCallback)
{
    _callbacks.push_back(std::make_pair(context, valueChagedCallback));
}

inline void DHTSensor::Init() 
{ 
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

        if(check) 
            for (const auto& callback : _callbacks)
                {
                    callback.second(callback.first, &_temperature, &_humidity);
                }
                
        _last_read = millis();
    }
}

inline unsigned short DHTSensor::readDelay(const unsigned short *delay)
{
    if(delay)
    {
        _read_delay = *delay;
        return 0;
    }
    return _read_delay;
}

unsigned short DHTSensor::wrapperReadDelay(const void* context, const unsigned short *delay)
{
    DHTSensor* obj = (DHTSensor*)context;

    return obj->readDelay(delay);
}

inline std::pair<float, float> DHTSensor::getLastData() { return std::make_pair(_temperature, _humidity); }
