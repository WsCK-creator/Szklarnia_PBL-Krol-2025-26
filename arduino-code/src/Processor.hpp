#pragma once 

#include <Arduino.h>

#include "SoilSensorState.hpp"
#include "DataTypes.hpp"

#include "DHTSensor.hpp"
#include "SoilSensor.hpp"
#include "WaterLevelSensor.hpp"
#include "Light.hpp"
#include "Relays.hpp"

class Processor
{
private:
    const char* _pump_setpoints[3] = {"MOKRY", "WILGOTNY", "SUCHY"};

    //#pragma region Type Config
    const ConfigFloat _temp_setpoint_cf = {wrapperTempSetpoint, -10, 50, 0.5, "C"};
    const ConfigFloat _temp_hys_cf = {wrapperTempHys, 0, 20, 0.5, "C"};
    const ConfigFloat _hum_setpoint_cf = {wrapperHumSetpoint, 0, 100, 1, "%"};
    const ConfigFloat _hum_hys_cf = {wrapperHumHys, 0, 50, 1, "%"};

    const ConfigUChar _pump_sensor_active_count_cf = {wrapperPumpSensorActiveCount, 1, 3, 1, ""};
    const ConfigEnum _pump_setpoint_cf = {wrapperPumpSetpoint, _pump_setpoints, 3 - 1};
    const ConfigUChar _pump_run_time_cf = {wrapperPumpRunTime, 1, 255, 1, "s"};
    const ConfigUChar _pump_run_interval_cf = {wrapperPumpRunInterval, 0, 255, 1, "s"};

    const ConfigUChar _led_treshold_cf = {wrapperLedTreshold, 0, 100, 1, "%"};
    const ConfigUChar _led_hys_cf = {wrapperLedHys, 0, 50, 1, "%"};
    const ConfigUShort _led_run_time_cf = {wrapperLedRunTime, 5, 1440, 5, "min"};
    const ConfigUShort _led_run_interval_cf = {wrapperLedRunInterval, 0, 1440, 5, "min"};
    //#pragma endregion

    float _temp_setpoint = 24; //c
    float _temp_hys = 2; //c
    float _hum_setpoint = 55; //%
    float _hum_hys = 5; //%

    unsigned char _pump_sensor_active_count = 2;
    SoilSensorState _pump_setpoint = SOIL_DRY;
    unsigned char _pump_run_time = 10; //s
    unsigned char _pump_run_interval = 10; //s

    unsigned char _led_treshold = 55; //%
    unsigned char _led_hys = 10; //%
    unsigned short _led_run_time = 120; //min
    unsigned short _led_run_interval = 120; //min

    DHTSensor* _dht_in = nullptr;
    DHTSensor* _dht_out = nullptr;
    SoilSensor* _soil[3] = {nullptr, nullptr, nullptr};
    WaterLevelSensor* _water = nullptr;
    Light* _light = nullptr;
    Relays* _relays = nullptr;

    SoilSensorState _soil_state[3] = {UNINITIALIZED, UNINITIALIZED, UNINITIALIZED};

    bool _pump_active = false;
    unsigned long _pump_time = 0;
    unsigned long _pump_delay = 0;

    bool _led_active = false;
    unsigned long _led_time = 0;
    unsigned long _led_delay = 0;
    
    void _checkSoil();
    void _dhtInCahnged(const float* temp, const float* hum);
    void _soilChanged(const unsigned char* id, const SoilSensorState* state);
    void _lightChanged(const unsigned char* level);
    void _waterChanged(const unsigned char* level);

    static void _wrapperDHTInCahnged(const void* context, const float* temp, const float* hum);
    static void _wrapperSoilChanged(const void* context, const unsigned char* id, const SoilSensorState* state);
    static void _wrapperLightChanged(const void* context, const unsigned char* level);
    static void _wrapperWaterChanged(const void* context, const unsigned char* level);

public:
    //#pragma region Data Config
    const DataConfig tempSetpointConfig = {TYPE_FLOAT, {.confFloat = &_temp_setpoint_cf}};
    const DataConfig tempHysConfig = {TYPE_FLOAT, {.confFloat = &_temp_hys_cf}};
    const DataConfig humSetpointConfig = {TYPE_FLOAT, {.confFloat = &_hum_setpoint_cf}};
    const DataConfig humHysConfig = {TYPE_FLOAT, {.confFloat = &_hum_hys_cf}};

    const DataConfig pumpSensorActiveCountConfig = {TYPE_UCHAR, {.confUChar = &_pump_sensor_active_count_cf}};
    const DataConfig pumpSetpointConfig = {TYPE_ENUM, {.confEnum = &_pump_setpoint_cf}};
    const DataConfig pumpRunTimeConfig = {TYPE_UCHAR, {.confUChar = &_pump_run_time_cf}};
    const DataConfig pumpRunIntervalConfig = {TYPE_UCHAR, {.confUChar = &_pump_run_interval_cf}};

    const DataConfig ledTresholdConfig = {TYPE_UCHAR, {.confUChar = &_led_treshold_cf}};
    const DataConfig ledHysConfig = {TYPE_UCHAR, {.confUChar = &_led_hys_cf}};
    const DataConfig ledRunTimeConfig = {TYPE_USHORT, {.confUShort = &_led_run_time_cf}};
    const DataConfig ledRunIntervalConfig = {TYPE_USHORT, {.confUShort = &_led_run_interval_cf}};
    //#pragma endregion

    Processor();
    void Init(DHTSensor* dhtIn, DHTSensor* dhtOut, SoilSensor* soil1, SoilSensor* soil2, 
    SoilSensor* soil3, WaterLevelSensor* water, Light* light, Relays* relays);

    void update();

    //#pragma region Settings Declaration

    inline float tempSetpoint(const float* temp);
    inline float tempHys(const float* temp);
    inline float humSetpoint(const float* perc);
    inline float humHys(const float* perc);

    inline unsigned char pumpSensorActiveCount(const unsigned char* count);
    inline short pumpSetpoint(const short* point);
    inline unsigned char pumpRunTime(const unsigned char* time);
    inline unsigned char pumpRunInterval(const unsigned char* time);

    inline unsigned char ledTreshold(const unsigned char* perc);
    inline unsigned char ledHys(const unsigned char* perc);
    inline unsigned short ledRunTime(const unsigned short* time);
    inline unsigned short ledRunInterval(const unsigned short* time);

    static float wrapperTempSetpoint(const void* context, const float* temp);
    static float wrapperTempHys(const void* context, const float* temp);
    static float wrapperHumSetpoint(const void* context, const float* perc);
    static float wrapperHumHys(const void* context, const float* perc);

    static unsigned char wrapperPumpSensorActiveCount(const void* context, const unsigned char* count);
    static short wrapperPumpSetpoint(const void* context, const short* point);
    static unsigned char wrapperPumpRunTime(const void* context, const unsigned char* time);
    static unsigned char wrapperPumpRunInterval(const void* context, const unsigned char* time);

    static unsigned char wrapperLedTreshold(const void* context, const unsigned char* perc);
    static unsigned char wrapperLedHys(const void* context, const unsigned char* perc);
    static unsigned short wrapperLedRunTime(const void* context, const unsigned short* time);
    static unsigned short wrapperLedRunInterval(const void* context, const unsigned short* time);
    //#pragma endregion
};

void Processor::_checkSoil()
{
    if(_soil_state[0] != UNINITIALIZED && _soil_state[1] != UNINITIALIZED && _soil_state[2] != UNINITIALIZED)
    {
        unsigned char count = 0;
        for (unsigned char i = 0; i < 3; i++)
            if(_soil_state[0] >= _pump_setpoint) count++;
        if(count >= _pump_sensor_active_count)
            _pump_active = true;
        else
            _pump_active = false;
        _light->bLED(&_pump_active);
    }
}

void Processor::_dhtInCahnged(const float *temp, const float *hum)
{
    bool enable;
    if(_relays->heater(nullptr))
    {
        if(*temp > _temp_setpoint + _temp_hys) enable = false;
    }
    else 
    {
        if(*temp < _temp_setpoint - _temp_hys) enable = true;
    }
    _relays->heater(&enable);
    _light->rLED(&enable);

    float tempOut = _dht_out->getLastData().first;
    short direction = _relays->actuator(nullptr);

    if(direction == UNKNOWN || direction == FINISHED) direction = CLOSE;
    else if(direction == OPEN || direction == FINISHED_OPEN)
    {
        if((tempOut > _temp_setpoint + _temp_hys && *hum < _hum_setpoint + _hum_hys)
            || (*temp < _temp_setpoint - _temp_hys && tempOut < _temp_setpoint + _temp_hys))
            direction = CLOSE;
        else direction = UNKNOWN;
    }
    else
    {
        if((*hum > _hum_setpoint + _hum_hys && tempOut > _temp_setpoint - _temp_hys)
            || (*temp > _temp_setpoint + _temp_hys && tempOut > _temp_setpoint - _temp_hys)
            || (*temp < _temp_setpoint - _temp_hys && tempOut > _temp_setpoint + _temp_hys))
            direction = OPEN;
        else direction = UNKNOWN;
    }
    _relays->actuator(&direction);
}

inline void Processor::_soilChanged(const unsigned char *id, const SoilSensorState *state)
{
    _soil_state[*id] = *state;
    _checkSoil();
    update();
}

inline void Processor::_lightChanged(const unsigned char *level)
{
    if(*level < _led_treshold - _led_hys)
        _led_active = true;
    else if(*level > _led_treshold + _led_hys)
        _led_active = false;
    update();
}

inline void Processor::_waterChanged(const unsigned char *level)
{
    bool enable;
    if(*level <= 15)
        enable = true;
    else
        enable = false;
    _light->gLED(&enable);
}

void Processor::_wrapperDHTInCahnged(const void* context, const float* temp, const float* hum)
{
    Processor* obj = (Processor*)context;
    return obj->_dhtInCahnged(temp, hum);
}

void Processor::_wrapperSoilChanged(const void *context, const unsigned char *id, const SoilSensorState *state)
{
    Processor* obj = (Processor*)context;
    return obj->_soilChanged(id, state);
}

void Processor::_wrapperLightChanged(const void *context, const unsigned char *level)
{
    Processor* obj = (Processor*)context;
    return obj->_lightChanged(level);
}

void Processor::_wrapperWaterChanged(const void *context, const unsigned char *level)
{
    Processor* obj = (Processor*)context;
    return obj->_waterChanged(level);
}

Processor::Processor() {}

void Processor::Init(DHTSensor* dhtIn, DHTSensor* dhtOut, SoilSensor* soil1, SoilSensor* soil2, 
    SoilSensor* soil3, WaterLevelSensor* water, Light* light, Relays* relays)
{
    _dht_in = dhtIn;
    _dht_out = dhtOut;
    _soil[0] = soil1;
    _soil[1] = soil2;
    _soil[2] = soil3;
    _water = water;
    _light = light;
    _relays = relays;

    _dht_in->addCallback(this, _wrapperDHTInCahnged);
    _soil[0]->addCallback(this, _wrapperSoilChanged);
    _soil[1]->addCallback(this, _wrapperSoilChanged);
    _soil[2]->addCallback(this, _wrapperSoilChanged);
    _water->addCallback(this, _wrapperWaterChanged);
    _light->addCallback(this, _wrapperLightChanged);
}

void Processor::update()
{
    bool check = false;
    if(!_pump_active)
    {
        if(_relays->pump(nullptr) != check)
            _relays->pump(&check);
        if(_pump_delay != 0)
        {
            _pump_time = 0;
            _pump_delay = 0;
        }    
    }
    else if(millis() >= _pump_time + _pump_delay)
    {
        if(_pump_run_interval == 0 || _pump_delay == _pump_run_interval * 1000)
        {
            _pump_delay = _pump_run_time * 1000;
            check = true;
            _relays->pump(&check);
            _pump_time = millis();
        }
        else
        {
            _pump_delay = _pump_run_interval * 1000;
            check = false;
            _relays->pump(&check);
            _pump_time = millis();
        }
    }

    if(!_led_active)
    {
        if(_relays->led(nullptr) != check)
            _relays->led(&check);
        if(_led_delay != 0)
        {
            _led_time = 0;
            _led_delay = 0;
        }
    }
    else if(millis() >= _led_time + _led_delay)
    {
        if(_led_run_interval == 0 ||_led_delay == _led_run_interval * 60 * 1000)
        {
            _led_delay = _led_run_time * 60 * 1000;
            check = true;
            _relays->led(&check);
            _led_time = millis();
        }
        else
        {
            _led_delay = _led_run_interval * 60 * 1000;
            check = false;
            _relays->led(&check);
            _led_time = millis();
        }
    }
}

//#pragma region settings

inline float Processor::tempSetpoint(const float* temp) {
    if (temp) _temp_setpoint = *temp;
    return _temp_setpoint;
}

inline float Processor::tempHys(const float* temp) {
    if (temp) _temp_hys = *temp;
    return _temp_hys;
}

inline float Processor::humSetpoint(const float* perc) {
    if (perc) _hum_setpoint = *perc;
    return _hum_setpoint;
}

inline float Processor::humHys(const float* perc) {
    if (perc) _hum_hys = *perc;
    return _hum_hys;
}

inline unsigned char Processor::pumpSensorActiveCount(const unsigned char* count) {
    if (count) _pump_sensor_active_count = *count;
    return _pump_sensor_active_count;
}

inline short Processor::pumpSetpoint(const short* point) {
    if (point) _pump_setpoint = (SoilSensorState)*point;
    return _pump_setpoint;
}

inline unsigned char Processor::pumpRunTime(const unsigned char* time) {
    if (time) _pump_run_time = *time;
    return _pump_run_time;
}

inline unsigned char Processor::pumpRunInterval(const unsigned char* time) {
    if (time) _pump_run_interval = *time;
    return _pump_run_interval;
}

inline unsigned char Processor::ledTreshold(const unsigned char* perc) {
    if (perc) _led_treshold = *perc;
    return _led_treshold;
}

inline unsigned char Processor::ledHys(const unsigned char* perc) {
    if (perc) _led_hys = *perc;
    return _led_hys;
}

inline unsigned short Processor::ledRunTime(const unsigned short* time) {
    if (time) _led_run_time = *time;
    return _led_run_time;
}

inline unsigned short Processor::ledRunInterval(const unsigned short* time) {
    if (time) _led_run_interval = *time;
    return _led_run_interval;
}

float Processor::wrapperTempSetpoint(const void *context, const float *temp)
{
    Processor* obj = (Processor*)context;
    return obj->tempSetpoint(temp);
}

float Processor::wrapperTempHys(const void *context, const float *temp)
{
    Processor* obj = (Processor*)context;
    return obj->tempHys(temp);
}

float Processor::wrapperHumSetpoint(const void *context, const float *perc)
{
    Processor* obj = (Processor*)context;
    return obj->humSetpoint(perc);
}

float Processor::wrapperHumHys(const void *context, const float *perc)
{
    Processor* obj = (Processor*)context;
    return obj->humHys(perc);
}

unsigned char Processor::wrapperPumpSensorActiveCount(const void *context, const unsigned char *count)
{
    Processor* obj = (Processor*)context;
    return obj->pumpSensorActiveCount(count);
}

short Processor::wrapperPumpSetpoint(const void *context, const short *point)
{
    Processor* obj = (Processor*)context;
    return obj->pumpSetpoint(point);
}

unsigned char Processor::wrapperPumpRunTime(const void *context, const unsigned char *time)
{
    Processor* obj = (Processor*)context;
    return obj->pumpRunTime(time);
}

unsigned char Processor::wrapperPumpRunInterval(const void *context, const unsigned char *time)
{
    Processor* obj = (Processor*)context;
    return obj->pumpRunInterval(time);
}

unsigned char Processor::wrapperLedTreshold(const void *context, const unsigned char *perc)
{
    Processor* obj = (Processor*)context;
    return obj->ledTreshold(perc);
}

unsigned char Processor::wrapperLedHys(const void *context, const unsigned char *perc)
{
    Processor* obj = (Processor*)context;
    return obj->ledHys(perc);
}

unsigned short Processor::wrapperLedRunTime(const void *context, const unsigned short *time)
{
    Processor* obj = (Processor*)context;
    return obj->ledRunTime(time);
}

unsigned short Processor::wrapperLedRunInterval(const void *context, const unsigned short *time)
{
    Processor* obj = (Processor*)context;
    return obj->ledRunInterval(time);
}

//#pragma endregion