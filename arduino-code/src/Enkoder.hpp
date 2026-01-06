#pragma once

#include <ArduinoSTL.h>

#define ENKODER_TIMEOUT 1000 //ms
#define BUTTON_DEBOUNCE_DELAY 200 //ms

enum Direction{
    LEFT,
    RIGHT
};

using EnkoderBtCallback = void (*)(const void*);
using EnkoderTurnedCallback = void (*)(const void*, const Direction*, const int*);


class Enkoder
{
private:
    std::vector<std::pair<const void*, EnkoderBtCallback>> _btCallbacks;
    std::vector<std::pair<const void*, EnkoderTurnedCallback>> _turnedCallbacks;

    unsigned char _pin_CLK;
    unsigned char _pin_DT;
    unsigned char _pin_SW;
    volatile int _position = 0;
    volatile Direction _direction;
    volatile unsigned long _lastEncChange = 0;
    volatile unsigned long _lastBtnPress = 0;
    volatile bool _encChanged = false;
    volatile bool _btnChanged = false;

    void _doButton();
    void _doEncoder();

    static Enkoder* _instance;
    static void _wrapperDoEncoder();
    static void _wrapperDoButton();

public:
    Enkoder(unsigned char pin_CLK, unsigned char pin_DT, unsigned char pin_SW);

    ~Enkoder()
    {
        detachInterrupt(digitalPinToInterrupt(_pin_CLK));
        detachInterrupt(digitalPinToInterrupt(_pin_SW));
    }

    void Init();
    int getEncPos();
    void addButtonCallback(const void* context, EnkoderBtCallback callback);
    void addTurnedCallback(const void* context, EnkoderTurnedCallback callback);

    void loop();
};

void Enkoder::_doButton()
{
    if(millis() - _lastBtnPress >= BUTTON_DEBOUNCE_DELAY || _lastBtnPress == 0)
    {
        _lastBtnPress = millis();
        _btnChanged = true;
    }
}

void Enkoder::_doEncoder()
{
        //if(millis() - _lastEncChange >= ENKODER_TIMEOUT) _position = 0;
        if (digitalRead(_pin_CLK) != digitalRead(_pin_DT)) {
            _direction = RIGHT;
            _position++; 
        } else {
            _direction = LEFT;
            _position--;
        }
        //_lastEncChange = millis();
        _encChanged = true;
    }

    void Enkoder::_wrapperDoEncoder()
    {
        if (_instance) _instance->_doEncoder();
    }
    
    void Enkoder::_wrapperDoButton()
    {
        if (_instance) _instance->_doButton();
    }

Enkoder* Enkoder::_instance = nullptr;

Enkoder::Enkoder(unsigned char pin_CLK, unsigned char pin_DT, unsigned char pin_SW)
{
    _pin_CLK = pin_CLK;
    _pin_DT = pin_DT;
    _pin_SW = pin_SW;
    _instance = this;
}

void Enkoder::Init()
{
    pinMode(_pin_CLK, INPUT_PULLUP);
    pinMode(_pin_DT, INPUT_PULLUP);
    pinMode(_pin_SW, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(_pin_CLK), Enkoder::_wrapperDoEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_pin_SW), Enkoder::_wrapperDoButton, FALLING);

    Serial.println("Enkoder initialized");
}

int Enkoder::getEncPos()
{
    //if(millis() - _lastEncChange > ENKODER_TIMEOUT) _position = 0;
    return _position;
}

void Enkoder::addButtonCallback(const void *context, EnkoderBtCallback callback)
{
    _btCallbacks.push_back(std::make_pair(context, callback));
}

void Enkoder::addTurnedCallback(const void *context, EnkoderTurnedCallback callback)
{
    _turnedCallbacks.push_back(std::make_pair(context, callback));
}

void Enkoder::loop()
{
    if(_encChanged)
    {
        _encChanged = false;
        Direction d = _direction;
        int p = _position;
        for (const auto &callback : _turnedCallbacks)
        {
            callback.second(callback.first, &d, &p);
        }
        _position = 0;
        Serial.print("Encoder turned:");
        Serial.println(p);
    }   
    if(_btnChanged)
    {
        _btnChanged = false;
        for (const auto &callback : _btCallbacks)
        {
            callback.second(callback.first);
        }
        Serial.println("Encoder button pressed");
    }
}
