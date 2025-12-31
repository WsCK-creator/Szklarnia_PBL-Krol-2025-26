#pragma once

#include <Arduino.h>
#include "DataTypes.hpp"

#define RELAY_ACTUATOR_PIN 46
#define RELAY_DIRECTION_PIN 45
#define RELAY_PUMP_PIN 44
#define RELAY_LED_PIN 43
#define RELAY_HEATER_PIN 42

enum ActuatorDirection{
    UNKNOWN = -1,
    CLOSE,
    OPEN,
    FINISHED,
    FINISHED_CLOSE,
    FINISHED_OPEN
};

using RelaysCallback = void (*)(const void*, const ActuatorDirection*, const bool*, const bool*, const bool*);

const char* MODE_STR[6] = {"NIEZNANY", "ZAMKNIJ", "OTWORZ", "ZAKONCZONO", "Z_ZAMKNIETE", "Z_OTWARTE"};

class Relays
{
private:
    const char* _actuator_mode[3] = {"ZAMKNIJ", "OTWORZ", "STOP"};
    const ConfigEnum _direction_cf = {wrapperActuator, _actuator_mode, FINISHED};
    const ConfigBool _led_cf = {wrapperLED, "ON", "OFF"};
    const ConfigBool _heater_cf = {wrapperHeater, "ON", "OFF"};
    const ConfigBool _pump_cf = {wrapperPump, "ON", "OFF"};
    const ConfigUChar _realy_delay_cf = {wrapperRelayDelay, 25, 255, 1, "ms"};
    const ConfigUChar _relay_off_delay_cf = {wrapperRelayOffDelay, 1, 255, 1, "s"};

    bool _pump_state = false;
    bool _led_state = false;
    bool _heater_state = false;
    bool _toCall = false;
    unsigned long _last_read = 0;
    unsigned long _delay = 0;
    unsigned char _realy_delay = 50; //ms
    unsigned char _relay_off_delay = 30; //s
    
    ActuatorDirection _actuator_state = UNKNOWN;
    ActuatorDirection _current_actuator_state = UNKNOWN;
    std::vector<std::pair<const void*, RelaysCallback>> _callbacks;
    
    void _run_actuator(ActuatorDirection open);
    void _stop_actuator();
    void _callCallbacks();
    
public:
    const DataConfig actuatorConfig = {TYPE_ENUM, {.confEnum = &_direction_cf}};
    const DataConfig ledConfig = {TYPE_BOOL, {.confBool = &_led_cf}};
    const DataConfig heaterConfig = {TYPE_BOOL, {.confBool = &_heater_cf}};
    const DataConfig pumpConfig = {TYPE_BOOL, {.confBool = &_pump_cf}};
    const DataConfig relayDelayConfig = {TYPE_UCHAR, {.confUChar = &_realy_delay_cf}};
    const DataConfig relayOffDelayConfig = {TYPE_UCHAR, {.confUChar = &_relay_off_delay_cf}};

    Relays();
    void Init();
    void update();
    void addCallback(const void* context, RelaysCallback callback);
    
    short actuator(const short* mode);
    bool led(const bool* check);
    bool heater(const bool* check);
    bool pump(const bool* check);
    unsigned char relayDelay(const unsigned char* delay);
    unsigned char relayOffDelay(const unsigned char* delay);
    
    static short wrapperActuator(const void* context, const short* mode);
    static bool wrapperLED(const void* context, const bool* mode);
    static bool wrapperHeater(const void* context, const bool* mode);
    static bool wrapperPump(const void* context, const bool* mode);
    static unsigned char wrapperRelayDelay(const void* context, const unsigned char* delay);
    static unsigned char wrapperRelayOffDelay(const void* context, const unsigned char* delay);
};

inline void Relays::_run_actuator(ActuatorDirection mode)
{
    if(mode == FINISHED) _stop_actuator();
    else
    {
        _actuator_state = mode;
        _current_actuator_state = UNKNOWN;
        update();
    }
}

inline void Relays::_stop_actuator()
{
    digitalWrite(RELAY_ACTUATOR_PIN, false);
    digitalWrite(RELAY_DIRECTION_PIN, false);
    _delay = _realy_delay;
    _current_actuator_state = FINISHED;
    _toCall = true;
}

void Relays::_callCallbacks()
{
    _toCall = false;
    for (const auto& callback : _callbacks)
    {
        callback.second(callback.first, &_actuator_state, &_led_state, &_heater_state, &_pump_state);
    }
}

Relays::Relays() {}

inline void Relays::Init()
{
    pinMode(RELAY_ACTUATOR_PIN, OUTPUT);
    pinMode(RELAY_DIRECTION_PIN, OUTPUT);
    pinMode(RELAY_PUMP_PIN, OUTPUT);
    pinMode(RELAY_LED_PIN, OUTPUT);
    pinMode(RELAY_HEATER_PIN, OUTPUT);
    digitalWrite(RELAY_ACTUATOR_PIN, false);
    digitalWrite(RELAY_DIRECTION_PIN, false);
    digitalWrite(RELAY_PUMP_PIN, false);
    digitalWrite(RELAY_LED_PIN, false);
    digitalWrite(RELAY_HEATER_PIN, false);
}

void Relays::update()
{
    if(_actuator_state != _current_actuator_state && millis() > _last_read + _delay)
    {
        if(_current_actuator_state == FINISHED) //finishing
        {
            _delay = 0;
            _actuator_state = (_actuator_state == OPEN) ? FINISHED_OPEN : FINISHED_CLOSE;
            _toCall = true;
        }
        else if(digitalRead(RELAY_ACTUATOR_PIN)) //is motor running
        {
            digitalWrite(RELAY_ACTUATOR_PIN, false);
            _delay = _realy_delay;
            _toCall = true;
        }
        else
        {
            if(digitalRead(RELAY_DIRECTION_PIN) !=  _actuator_state)
            {
                digitalWrite(RELAY_DIRECTION_PIN,  _actuator_state);
                _delay = _realy_delay;
                _toCall = true;
            }
            else
            {
                digitalWrite(RELAY_ACTUATOR_PIN, true);
                _delay = _relay_off_delay * 60 * 1000;
                _current_actuator_state = _actuator_state;
                _toCall = true;
            }
        }
    }
    else if (_delay != 0 && millis() > _last_read + _delay) _stop_actuator();
    if(_toCall) _callCallbacks();
}

void Relays::addCallback(const void *context, RelaysCallback callback)
{
    _callbacks.push_back(std::make_pair(context, callback));
}

inline short Relays::actuator(const short* mode)
{
    if(mode)
    {
        if(*mode < UNKNOWN || *mode > FINISHED) return ActuatorDirection();
        if(*mode != UNKNOWN) _run_actuator((ActuatorDirection)*mode);
        return ActuatorDirection();
    }
    else return _actuator_state;
}

inline bool Relays::led(const bool* check)
{
    if(check) {
        if(_led_state != *check) {
            digitalWrite(RELAY_LED_PIN, *check);
            _led_state = *check;
            _toCall = true;
        }
    }
    return _led_state;
}

inline bool Relays::heater(const bool* check)
{
    if(check)
        if(_heater_state != *check){
            digitalWrite(RELAY_HEATER_PIN, *check);
            _heater_state = *check;
            _toCall = true;
        }

    return _heater_state;
}

inline bool Relays::pump(const bool* check)
{
    if (check)
        if(_pump_state != *check){
            digitalWrite(RELAY_PUMP_PIN, *check);
            _pump_state = *check;
            _toCall = true;
        }

    return _pump_state;
}

inline unsigned char Relays::relayDelay(const unsigned char *delay)
{
    if(delay) _realy_delay = *delay;
    return _realy_delay;
}

inline unsigned char Relays::relayOffDelay(const unsigned char *delay)
{
    if(delay) _relay_off_delay = *delay;
    return _relay_off_delay;
}


short Relays::wrapperActuator(const void* context, const short *mode )
{
    Relays* obj = (Relays*)context;
    return obj->actuator(mode);
}

bool Relays::wrapperLED(const void *context, const bool *check)
{
    Relays* obj = (Relays*)context;
    return obj->led(check);
}

bool Relays::wrapperHeater(const void *context, const bool *check)
{
    Relays* obj = (Relays*)context;
    return obj->heater(check);
}

bool Relays::wrapperPump(const void *context, const bool *check)
{
    Relays* obj = (Relays*)context;
    return obj->pump(check);
}

inline unsigned char Relays::wrapperRelayDelay(const void *context, const unsigned char *delay)
{
    Relays* obj = (Relays*)context;
    return obj->relayDelay(delay);
}

inline unsigned char Relays::wrapperRelayOffDelay(const void *context, const unsigned char *delay)
{
    Relays* obj = (Relays*)context;
    return obj->relayOffDelay(delay);
}
