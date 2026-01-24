#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include "SoilSensorState.hpp"
#include "DataTypes.hpp"
#include "MenuItems.hpp"

#include "virtuabotixRTC.h"
#include "Enkoder.hpp"
#include "DHTSensor.hpp"
#include "SoilSensor.hpp"
#include "WaterLevelSensor.hpp"
#include "Light.hpp"
#include "Relays.hpp"
#include "Processor.hpp"

class Disp
{
private:
    const ConfigUChar _brightness_cf = {wrapperBrightness, 0, 255, 1, ""};
    const ConfigUChar _blanking_brightness_cf = {wrapperBlankingBrightness, 0, 255, 1, ""};
    const ConfigUChar _blanking_time_cf = {wrapperBlankingTime, 0, 255, 1, "min"};
    const ConfigUChar _screan_switch_time_cf = {wrapperScreanSwitchTime, 0, 255, 1, "s"};
    
    virtuabotixRTC* _rtc = nullptr;
    Enkoder* _enkoder = nullptr;
    DHTSensor* _dht_in = nullptr;
    DHTSensor* _dht_out = nullptr;
    SoilSensor* _soil[3] = {nullptr, nullptr, nullptr};
    WaterLevelSensor* _water = nullptr;
    Light* _light = nullptr;
    Relays* _relays = nullptr;
    Processor* _processor = nullptr;
    const void* _father = nullptr;

    unsigned char _brightness = 200;
    unsigned char _blanking_brightness = 75;
    unsigned char _blanking_time = 5;       // min
    unsigned long _blanking_start = 0;
    unsigned char _screan_switch_time = 10; // s
    unsigned long _last_switch_time = 0;
    unsigned char _screen_num = 1;
    unsigned char _back_to_switching_time = 90; // s
    bool _turned = false;
    bool _manual_mode = false;
    String _time = "00:00";
    String _date = "01.01.2025";
    float _temp_in = 0;
    float _temp_out = 0;
    float _hum_in = 0;
    float _hum_out = 0;
    unsigned char _light_level = 100;
    SoilSensorState _soil_state[3] = {UNINITIALIZED, UNINITIALIZED, UNINITIALIZED};
    unsigned char _water_level = 0;
    bool _LED_state = false;
    bool _heater_state = false;
    ActuatorDirection _actuator_state = UNKNOWN;
    unsigned char _menuIndex = 0;
    const MenuItem *_curentItem = nullptr;
    double _value = 0;
    double _min = 0;
    double _max = 0;
    double _step = 0;

    const DataConfig* _curentConfig = nullptr;

    U8G2_SSD1327_VISIONOX_128X96_1_4W_HW_SPI u8g2;
    void _setTime();
    void _showScreen();
    void _showMenu(const MenuItem *item);
    void _drawStatusLine(int y, const char *label, bool state);
    void _drawStatusLine(int y, const char *label, ActuatorDirection dir);
    void _screen1_Clock();
    void _screen2_Climate();
    void _screen3_Soil();
    void _screen4_Water();
    void _screen5_Status();
    void _screen6_Settings(const MenuItem *item);
    void _screen7_SimpleEditor(String value);

    void _getCurrentConfig(const MenuID* id);
    void _dispScr7();
    void _saveConfig(bool exit = false);
    void _changeSrc7(const Direction* direction, const int* position);
    void _encPressed();
    void _encTurned(const Direction* direction, const int* position);
    void _setDHTIn(const float* temp, const float* hum);
    void _setDHTOut(const float* temp, const float* hum);
    void _setSoil(const unsigned char* chr, const SoilSensorState* state);
    void _setWater(const unsigned char* level);
    void _setLight(const unsigned char* level);
    void _setRelays(const ActuatorDirection* actuator, const bool* led, const bool* heater, const bool* pump);
    static void _wrapperEncPressed(const void* context);
    static void _wrapperEncTurned(const void* context, const Direction* direction, const int* position);
    static void _wrapperSetDHTIn(const void* context, const float* temp, const float* hum);
    static void _wrapperSetDHTOut(const void* context, const float* temp, const float* hum);
    static void _wrapperSetSoil(const void* context, const unsigned char* chr, const SoilSensorState* state);
    static void _wrapperSetWater(const void* context, const unsigned char* level);
    static void _wrapperSetLight(const void* context, const unsigned char* level);
    static void _wrapperSetRelays(const void* context, const ActuatorDirection* actuator, const bool* led, const bool* heater, const bool* pump);

public:
    const DataConfig brightnessConfig = {TYPE_UCHAR, {.confUChar = &_brightness_cf}};
    const DataConfig blankingBrightnessConfig = {TYPE_UCHAR, {.confUChar = &_blanking_brightness_cf}};
    const DataConfig blankingTimeConfig = {TYPE_UCHAR, {.confUChar = &_blanking_time_cf}};
    const DataConfig screanSwitchTimeConfig = {TYPE_UCHAR, {.confUChar = &_screan_switch_time_cf}};

    Disp(unsigned char cs, unsigned char rst, unsigned char dc);
    void Init(DHTSensor* dhtIn, DHTSensor* dhtOut, SoilSensor* soil1, SoilSensor* soil2, 
    SoilSensor* soil3, WaterLevelSensor* water, Light* light, Relays* relays, Enkoder *enkoder, virtuabotixRTC *rtc, Processor* processor);
    void update();

    unsigned char brightness(const unsigned char* val);
    unsigned char blankingBrightness(const unsigned char* val);
    unsigned char blankingTime(const unsigned char* time);
    unsigned char screanSwitchTime(const unsigned char* time);

    static unsigned char wrapperBrightness(const void* context, const unsigned char* val);
    static unsigned char wrapperBlankingBrightness(const void* context, const unsigned char* val);
    static unsigned char wrapperBlankingTime(const void* context, const unsigned char* time);
    static unsigned char wrapperScreanSwitchTime(const void* context, const unsigned char* time);
};

void Disp::_setTime()
{
    _rtc->updateTime();
    _time = String(_rtc->hours) + ":" + String(_rtc->minutes);
    _date = String(_rtc->dayofmonth) + "." + String(_rtc->month) + "." + String(_rtc->year);
}

void Disp::_showScreen()
{
    if (_screen_num == 1) _setTime();
    u8g2.firstPage();
    do
    {
        switch (_screen_num)
        {
        case 1:
            _screen1_Clock();
            break;
        case 2:
            _screen2_Climate();
            break;
        case 3:
            _screen3_Soil();
            break;
        case 4:
            _screen4_Water();
            break;
        case 5:
            _screen5_Status();
            break;
        }
    } while (u8g2.nextPage());
    _last_switch_time = millis();
}

void Disp::_showMenu(const MenuItem *item)
{
    if(_curentConfig)
    {
        _curentConfig = nullptr;
        _father = nullptr;
        _value = 0;
        _min = 0;
        _max = 0;
        _step = 0;
        _menuIndex = 0;
    }
    u8g2.firstPage();
    do
    {
        _screen6_Settings(item);
    } while (u8g2.nextPage());
    _last_switch_time = millis();
}

void Disp::_drawStatusLine(int y, const char *label, bool state)
{
    u8g2.drawFrame(0, y, 128, 15); // Ramka
    u8g2.setFont(u8g2_font_helvB08_tr);

    // Nazwa urządzenia po lewej
    u8g2.drawStr(5, y + 11, label);

    // Status po prawej
    if (state)
    {
        // Jeśli ON -> Rysujemy czarny kwadrat (Box)
        u8g2.drawBox(70, y, 58, 15);
        u8g2.setDrawColor(0);                 // Zmieniamy kolor pędzla na "gumkę" (czarny)
        u8g2.drawStr(75, y + 11, "WLACZONE"); // Tekst wycięty w tle
        u8g2.setDrawColor(1);                 // Powrót do normalnego rysowania
    }
    else
    {
        // Jeśli OFF -> Zwykły tekst
        u8g2.drawStr(85, y + 11, "WYL");
    }
}

inline void Disp::_drawStatusLine(int y, const char *label, ActuatorDirection dir)
{
    u8g2.drawFrame(0, y, 128, 15); // Ramka
    u8g2.setFont(u8g2_font_helvB08_tr);

    // Nazwa urządzenia po lewej
    u8g2.drawStr(5, y + 11, label);

    // Status po prawej
    if (dir == OPEN || dir == CLOSE || dir == UNKNOWN)
    {
        // Jeśli ON -> Rysujemy czarny kwadrat (Box)
        u8g2.drawBox(70, y, 58, 15);
        u8g2.setDrawColor(0);                 // Zmieniamy kolor pędzla na "gumkę" (czarny)
        u8g2.drawStr(75, y + 11, MODE_STR[dir + 1]); // Tekst wycięty w tle
        u8g2.setDrawColor(1);                 // Powrót do normalnego rysowania
    }
    else
    {
        // Jeśli OFF -> Zwykły tekst
        u8g2.drawStr(85, y + 11, MODE_STR[dir + 1]);
    }
}

// --- EKRAN 1: ZEGAR ---
void Disp::_screen1_Clock()
{
    u8g2.setFont(u8g2_font_ncenB18_tr);
    // Wyśrodkowanie godziny (orientacyjne dla 128px)
    int w = u8g2.getStrWidth(_time.c_str());
    u8g2.drawStr((128 - w) / 2, 50, _time.c_str());

    u8g2.setFont(u8g2_font_ncenB10_tr);
    w = u8g2.getStrWidth(_date.c_str());
    u8g2.drawStr((128 - w) / 2, 75, _date.c_str());

    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(50, 20, "CZAS");
}

// --- EKRAN 2: KLIMAT & ŚWIATŁO ---
void Disp::_screen2_Climate()
{
    // --- GÓRA: NA ZEWNĄTRZ ---
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(0, 10, "NA ZEWNATRZ:");

    u8g2.setFont(u8g2_font_helvR08_tr);
    u8g2.setCursor(0, 24);
    u8g2.print("T: ");
    u8g2.print(_temp_out, 1);
    u8g2.print("C   ");
    u8g2.print("W: ");
    u8g2.print(_hum_out);
    u8g2.print("%");

    // Pasek światła
    u8g2.setCursor(0, 38);
    u8g2.print("Swiatlo: ");
    u8g2.print(_light_level);
    u8g2.print("%");
    u8g2.drawFrame(65, 30, 60, 6);
    u8g2.drawBox(67, 32, (56 * _light_level) / 100, 2);

    // Linia podziału
    u8g2.drawLine(0, 45, 128, 45);

    // --- DÓŁ: SZKLARNIA (OBOK SIEBIE) ---
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(0, 58, "SZKLARNIA (SRODEK):");

    // LEWA STRONA: TEMPERATURA
    u8g2.setFont(u8g2_font_u8glib_4_tf); // Opis mikroskopijną czcionką nad liczbą
    u8g2.drawStr(10, 70, "TEMP");

    u8g2.setFont(u8g2_font_ncenB12_tr); // Duża liczba
    u8g2.setCursor(5, 88);
    u8g2.print(_temp_in, 1);
    u8g2.print("C");

    // PRAWA STRONA: WILGOTNOŚĆ
    u8g2.setFont(u8g2_font_u8glib_4_tf); // Opis mikroskopijną czcionką nad liczbą
    u8g2.drawStr(80, 70, "WILG");

    u8g2.setFont(u8g2_font_ncenB12_tr); // Duża liczba
    u8g2.setCursor(75, 88);
    u8g2.print(_hum_in);
    u8g2.print("%");
}

// --- EKRAN 3: GLEBA ---
void Disp::_screen3_Soil()
{
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(10, 10, "WILGOTNOSC GLEBY");

    // Rysujemy 3 słupki obok siebie
    // Pozycje X dla słupków: 15, 55, 95

    for (unsigned char i = 0; i < 3; i++)
    {
        u8g2.drawFrame(15 + 40 * i, 20, 20, 50); // Ramka
        int h = (_soil_state[i] <= SOIL_WET) ? 46 : (_soil_state[i] <= SOIL_MOIST) ? 25 : 5; // Wysokość wypełnienia
        u8g2.drawBox(17 + 40 * i, (70 - h), 16, h);             // Wypełnienie od dołu
        u8g2.setFont(u8g2_font_helvR08_tr);
        u8g2.drawStr(18 + 40 * i, 82, ("CZ " + String(i + 1)).c_str()); // Podpis
    }
    // Legenda na samym dole (opcjonalnie)
    // u8g2.setFont(u8g2_font_u8glib_4_tf); // Bardzo mała czcionka (micro)
    // u8g2.drawStr(15, 95, "MIN          MED          MAX");
}

// --- EKRAN 4: ZBIORNIK ---
void Disp::_screen4_Water()
{
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(45, 12, "ZBIORNIK");

    // Rysunek zbiornika
    u8g2.drawFrame(10, 20, 50, 70); // Obrys

    // Poziom wody (maks wys = 70px)
    int h = (70 * _water_level) / 100;
    if (h > 70)
        h = 70;
    // Rysowanie wypełnienia (od dołu: y_start = 20 + 70 - h)
    u8g2.drawBox(12, (90 - h), 46, h);

    // Tekst z procentami (duży)
    u8g2.setFont(u8g2_font_ncenB18_tr);
    u8g2.setCursor(70, 60);
    u8g2.print(_water_level);
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.print("%");
}

// --- EKRAN 5: STATUS URZĄDZEŃ ---
void Disp::_screen5_Status()
{
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(40, 10, "STATUSY");

    // Wywołanie pomocnika rysującego paski
    _drawStatusLine(20, "GRZALKA", _heater_state);
    _drawStatusLine(45, "KLAPA", _actuator_state);
    _drawStatusLine(70, "SWIATLO", _LED_state);
}

void Disp::_screen6_Settings(const MenuItem *item)
{
    _curentItem = item;
    // Nagłówek
    u8g2.setFont(u8g2_font_helvB10_tr);
    u8g2.drawStr(0, 14, item->header);
    u8g2.drawLine(0, 16, 128, 16);

    // LOGIKA PRZEWIJANIA (VIEWPORT)
    // Mamy miejsce na 4 linie (y = 20, 38, 56, 74)
    // Musimy obliczyć, który element jest "na górze" (topIndex)

    int maxVisible = 4;
    int topIndex = 0;

    // Jeśli zaznaczony element jest głębiej niż 4 pozycja, przesuwamy widok
    if (_menuIndex >= maxVisible)
    {
        topIndex = _menuIndex - (maxVisible - 1);
    }

    // Rysowanie Paska przewijania (Scrollbar) po prawej
    u8g2.drawFrame(124, 18, 4, 78); // Ramka paska
    // Obliczanie pozycji suwaka
    //int scrollHeight = 78 / item->count; // Wysokość suwaka
    //int scrollY = 18 + (topIndex * (78 - scrollHeight) / (item->count - maxVisible));
    // Prostsza wersja pozycji suwaka (liniowa):
    int scrollY_simple = 18 + (_menuIndex * 74 / item->count);
    u8g2.drawBox(125, scrollY_simple, 2, 6);

    // Rysowanie Listy
    u8g2.setFont(u8g2_font_helvR08_tr);

    for (int i = 0; i < maxVisible; i++)
    {
        int itemIndex = topIndex + i; // Który to element z tablicy

        // Sprawdzamy czy nie wyszliśmy poza tablicę
        if (itemIndex >= item->count)
            break;

        int yPos = 18 + (i * 18); // Pozycja Y linii (każda linia ma 18px wysokości)

        // Jeśli to jest zaznaczony element -> Rysujemy negatyw
        if (itemIndex == _menuIndex)
        {
            u8g2.drawBox(0, yPos, 122, 17); // Czarne tło (box)
            u8g2.setDrawColor(0);           // Biały tekst
        }

        // Rysowanie tekstu (x=4 dla marginesu)
        u8g2.drawStr(4, yPos + 12, item->items[itemIndex]->menuName);

        // Powrót do normalnego koloru
        u8g2.setDrawColor(1);
    }
}

void Disp::_screen7_SimpleEditor(String value) {
  // 1. HEADER (Czarny pasek na górze)
  u8g2.setDrawColor(1);
  u8g2.drawBox(0, 0, 128, 22); // Tło nagłówka (wysokość 22px)
  
  u8g2.setDrawColor(0); // Zmieniamy kolor na "wymazywanie" (biały tekst na czarnym)
  u8g2.setFont(u8g2_font_helvB10_tr); // Czcionka nagłówka
  
  // Centrowanie Nagłówka
  int headerW = u8g2.getStrWidth(_curentItem->header);
  u8g2.drawStr((128 - headerW) / 2, 16, _curentItem->header);
  
  // 2. WARTOŚĆ (Duża czcionka na środku)
  u8g2.setDrawColor(1); // Powrót do normalnego koloru (czarny tekst na białym)
  
  // Wybór dużej czcionki (Logisoso 24px)
  u8g2.setFont(u8g2_font_logisoso24_tr); 
  
  // Obliczanie pozycji, żeby było idealnie na środku pozostałego miejsca
  int valW = u8g2.getStrWidth(value.c_str());
  int xPos = (128 - valW) / 2;
  
  // Rysowanie wartości (Y=70 to optyczny środek pod nagłówkiem)
  u8g2.drawStr(xPos, 70, value.c_str());
}

void Disp::_getCurrentConfig(const MenuID *id)
{
    _manual_mode = false;
    switch (*id)
    {
        case ACTUATOR:
            _curentConfig = &_relays->actuatorConfig;
            _father = _relays;
            _manual_mode = true;
            break;
        case LED:
            _curentConfig = &_relays->ledConfig;
            _father = _relays;
            _manual_mode = true;
            break;
        case PUMP:
            _curentConfig = &_relays->pumpConfig;
            _father = _relays;
            _manual_mode = true;
            break;
        case HEATER:
            _curentConfig = &_relays->heaterConfig;
            _father = _relays;
            _manual_mode = true;
            break;
        
        case SWITCHING_DELAY:
            _curentConfig = &_relays->relayDelayConfig;
            _father = _relays;
            break;
        case SWITCHING_OFF_DELAY:
            _curentConfig = &_relays->relayOffDelayConfig;
            _father = _relays;
            break;
        
        case TEMP_SETPOINT:
            _curentConfig = &_processor->tempSetpointConfig;
            _father = _processor;
            break;
        case TEMP_HYS:
            _curentConfig = &_processor->tempHysConfig;
            _father = _processor;
            break;
        case HUM_SETPOINT:
            _curentConfig = &_processor->humSetpointConfig;
            _father = _processor;
            break;
        case HUM_HYS:
            _curentConfig = &_processor->humHysConfig;
            _father = _processor;
            break;

        case PUMP_SENSOR_COUNT:
            _curentConfig = &_processor->pumpSensorActiveCountConfig;
            _father = _processor;
            break;
        case PUMP_SETPOINT:
            _curentConfig = &_processor->pumpSetpointConfig;
            _father = _processor;
            break;
        case PUMP_RUN_TIME:
            _curentConfig = &_processor->pumpRunTimeConfig;
            _father = _processor;
            break;
        case PUMP_RUN_INTERVAL:
            _curentConfig = &_processor->pumpRunIntervalConfig;
            _father = _processor;
            break;
        
        case LED_THRESHOLD:
            _curentConfig = &_processor->ledTresholdConfig;
            _father = _processor;
            break;
        case LED_HYS:
            _curentConfig = &_processor->ledHysConfig;
            _father = _processor;
            break;
        case LED_RUN_TIME:
            _curentConfig = &_processor->ledRunTimeConfig;
            _father = _processor;
            break;
        case LED_RUN_INTERVAL:
            _curentConfig = &_processor->ledRunIntervalConfig;
            _father = _processor;
            break;

        case DISPLAY_BRIGHTNESS:
            _curentConfig = &brightnessConfig;
            _father = this;
            break;
        case DISPLAY_SAVER_BRIGHTNESS:
            _curentConfig = &blankingBrightnessConfig;
            _father = this;
            break;
        case DISPLAY_SAVER_TIME:
            _curentConfig = &blankingTimeConfig;
            _father = this;
            break;
        case DISPLAY_SWITCH_TIME:
            _curentConfig = &screanSwitchTimeConfig;
            _father = this;
            break;

        case SENSORS_SOIL1_DELAY:
            _curentConfig = &_soil[0]->delayConfig;
            _father = _soil[0];
            break;
        case SENSORS_SOIL1_HYS:
            _curentConfig = &_soil[0]->hysteresisConfig;
            _father = _soil[0];
            break;
        case SENSORS_SOIL2_DELAY:
            _curentConfig = &_soil[1]->delayConfig;
            _father = _soil[1];
            break;
        case SENSORS_SOIL2_HYS:
            _curentConfig = &_soil[1]->hysteresisConfig;
            _father = _soil[1];
            break;
        case SENSORS_SOIL3_DELAY:
            _curentConfig = &_soil[2]->delayConfig;
            _father = _soil[2];
            break;
        case SENSORS_SOIL3_HYS:
            _curentConfig = &_soil[2]->hysteresisConfig;
            _father = _soil[2];
            break;
        case SENSORS_DHT_IN:
            _curentConfig = &_dht_in->delayConfig;
            _father = _dht_in;
            break;
        case SENSORS_DHT_OUT:
            _curentConfig = &_dht_out->delayConfig;
            _father = _dht_out;
            break;
        case SENSORS_WATER:
            _curentConfig = &_water->delayConfig;
            _father = _water;
            break;
        case SENSORS_PHOTO:
            _curentConfig = &_light->delayConfig;
            _father = _light;
            break;
        
        default:
            _curentConfig = nullptr;
            _father = nullptr;
            break;
    }
}

void Disp::_dispScr7()
{
    _getCurrentConfig(&_curentItem->id);
    
    if(_curentConfig)
    {
        String str = "";
        switch (_curentConfig->type)
        {
            case TYPE_UCHAR:
            {
                _value = _curentConfig->ptr.confUChar->callback(_father, nullptr);
                _min = _curentConfig->ptr.confUChar->minVal;
                _max = _curentConfig->ptr.confUChar->maxVal;
                _step = _curentConfig->ptr.confUChar->step;

                str += String(_curentConfig->ptr.confUChar->callback(_father, nullptr), DEC);
                str += (_curentConfig->ptr.confUChar->unit[0] != '\0') ? String(" ") + _curentConfig->ptr.confUChar->unit : "";
                break;
            }
            case TYPE_USHORT:
            {
                _value = _curentConfig->ptr.confUShort->callback(_father, nullptr);
                _min = _curentConfig->ptr.confUShort->minVal;
                _max = _curentConfig->ptr.confUShort->maxVal;
                _step = _curentConfig->ptr.confUShort->step;

                str += String(_curentConfig->ptr.confUShort->callback(_father, nullptr), DEC);
                str += (_curentConfig->ptr.confUShort->unit[0] != '\0') ? String(" ") + _curentConfig->ptr.confUShort->unit : "";
                break;
            }
            case TYPE_FLOAT:
            {
                _value = _curentConfig->ptr.confFloat->callback(_father, nullptr);
                _min = _curentConfig->ptr.confFloat->minVal;
                _max = _curentConfig->ptr.confFloat->maxVal;
                _step = _curentConfig->ptr.confFloat->step;

                str += String(_curentConfig->ptr.confFloat->callback(_father, nullptr), DEC);
                str += (_curentConfig->ptr.confFloat->unit[0] != '\0') ? String(" ") + _curentConfig->ptr.confFloat->unit : "";
                break;
            }
            case TYPE_BOOL:
            {
                _value = _curentConfig->ptr.confBool->callback(_father, nullptr);
                _min = 0;
                _max = 1;
                _step = 1;

                bool check = _curentConfig->ptr.confBool->callback(_father, nullptr);
                str += (check) ? _curentConfig->ptr.confBool->txtOn : _curentConfig->ptr.confBool->txtOff;
                break;
            }
            case TYPE_ENUM:
            {
                _value = _curentConfig->ptr.confEnum->callback(_father, nullptr);
                _min = 0;
                _max = _curentConfig->ptr.confEnum->maxVal;
                _step = 1;

                short chr =  _curentConfig->ptr.confEnum->callback(_father, nullptr);
                str += _curentConfig->ptr.confEnum->options[chr];
                break;
            }
        }
        u8g2.firstPage();
        do
        {
            _screen7_SimpleEditor(str);
        } while (u8g2.nextPage());
    }
}

void Disp::_saveConfig(bool exit)
{
    if(_curentConfig)
    {
        switch (_curentConfig->type)
        {
            case TYPE_UCHAR:
                _curentConfig->ptr.confUChar->callback(_father, (unsigned char*)&_value);
                break;
            case TYPE_USHORT:
                _curentConfig->ptr.confUShort->callback(_father, (unsigned short*)&_value);
                break;
            case TYPE_FLOAT:
                _curentConfig->ptr.confFloat->callback(_father, (float*)&_value);
                break;
            case TYPE_BOOL:
                if(exit) _value = 0;
                _curentConfig->ptr.confBool->callback(_father, (bool*)&_value);
                break;
            case TYPE_ENUM:
                _curentConfig->ptr.confEnum->callback(_father, (short*)&_value);
                break;
        }
    }
}

void Disp::_changeSrc7(const Direction *direction, const int *position)
{
    if(_curentConfig && position && position != 0)
    {
        int pos = *position;
        int f = (pos > 0) ? -1 : 1;
        while (pos != 0)
        {
            pos += f;
            _value += _step * f * -1;
            if(_value > _max)
            {
                _value = _min;
            }
            else if(_value < _min)
            {
                _value = _max;
            }
        }
    }
    String str = "";
    switch (_curentConfig->type)
    {
        case TYPE_UCHAR:
            str += String(_value, 0);
            str += (_curentConfig->ptr.confUChar->unit[0] != '\0') ? String(" ") + _curentConfig->ptr.confUChar->unit : "";
            break;
        case TYPE_USHORT:
            str += String(_value, 0);
            str += (_curentConfig->ptr.confUShort->unit[0] != '\0') ? String(" ") + _curentConfig->ptr.confUShort->unit : "";
            break;
        case TYPE_FLOAT:
            str += String(_value, 1);
            str += (_curentConfig->ptr.confFloat->unit[0] != '\0') ? String(" ") + _curentConfig->ptr.confFloat->unit : "";
            break;
        case TYPE_BOOL:
            str += (_value != 0) ? _curentConfig->ptr.confBool->txtOn : _curentConfig->ptr.confBool->txtOff;
            break;
        case TYPE_ENUM:
            str += _curentConfig->ptr.confEnum->options[(int)_value];
            break;
    }
    u8g2.firstPage();
    do
    {
        _screen7_SimpleEditor(str);
    } while (u8g2.nextPage());
    _last_switch_time = millis();
    
    if(_manual_mode) _saveConfig();
}

void Disp::_encPressed()
{
    _blanking_start = millis();
    u8g2.setContrast(_brightness);
    if (!_curentItem)
    {
        _screen_num = 6;
        _showMenu(&mainMenu);
    }
    else
    {
        if(_curentItem->items)
        {
            if(_menuIndex == 0)
            {
                if (_curentItem->parent) _showMenu(_curentItem->parent);
                else
                {
                    _curentItem = nullptr;
                    _screen_num = 1;
                    _showScreen();
                }
            }
            else if(_curentItem->items[_menuIndex]->items) _showMenu(_curentItem->items[_menuIndex]);
            else _dispScr7();
        }
        else
        {
            if(_curentItem->id == PUMP_SETPOINT) _value = 250 + _value * 40;
            _saveConfig(true);
            _showMenu(_curentItem->parent);
        }
    }
}

void Disp::_encTurned(const Direction *direction, const int *position)
{
    _blanking_start = millis();
    u8g2.setContrast(_brightness);
    if (!_curentItem)
    {
        _turned = true;
        _screen_num++;
        if (_screen_num > 5)
            _screen_num = 1;
        _showScreen();
    }
    else if (_curentItem->count > 0)
    {
        if(*direction == RIGHT)
        {
            _menuIndex++;
            if(_menuIndex >= _curentItem->count)
                _menuIndex = 0;
        }
        else
        {
            if (_menuIndex == 0) _menuIndex = _curentItem->count - 1;
            else _menuIndex--;
        }
        _showMenu(_curentItem);
    }
    else _changeSrc7(direction, position);
}

inline void Disp::_setDHTIn(const float *temp, const float *hum)
{
    _temp_in = *temp;
    _hum_in = *hum;
}

inline void Disp::_setDHTOut(const float *temp, const float *hum)
{
    _temp_out = *temp;
    _hum_out = *hum;
}

inline void Disp::_setSoil(const unsigned char *chr, const SoilSensorState *state)
{
    _soil_state[*chr] = *state;
}

inline void Disp::_setWater(const unsigned char *level)
{
    _water_level = *level;
}

inline void Disp::_setLight(const unsigned char *level)
{
    _light_level = *level;
}

inline void Disp::_setRelays(const ActuatorDirection *actuator, const bool *led, const bool *heater, const bool *pump)
{
    _actuator_state = *actuator;
    _LED_state = *led;
    _heater_state = *heater;
    //_pump_state = *pump;
}

void Disp::_wrapperEncPressed(const void *context)
{
    Disp* obj = (Disp*)context;
    return obj->_encPressed();
}

void Disp::_wrapperEncTurned(const void *context, const Direction *direction, const int *position)
{
    Disp* obj = (Disp*)context;
    return obj->_encTurned(direction, position);
}

inline void Disp::_wrapperSetDHTIn(const void *context, const float *temp, const float *hum)
{
    Disp* obj = (Disp*)context;
    return obj->_setDHTIn(temp, hum);
}

inline void Disp::_wrapperSetDHTOut(const void *context, const float *temp, const float *hum)
{
    Disp* obj = (Disp*)context;
    return obj->_setDHTOut(temp, hum);
}

inline void Disp::_wrapperSetSoil(const void *context, const unsigned char *chr, const SoilSensorState *state)
{
    Disp* obj = (Disp*)context;
    return obj->_setSoil(chr, state);
}

void Disp::_wrapperSetWater(const void *context, const unsigned char *level)
{
    Disp* obj = (Disp*)context;
    obj->_setWater(level);
    return;
}

inline void Disp::_wrapperSetLight(const void *context, const unsigned char *level)
{
    Disp* obj = (Disp*)context;
    return obj->_setLight(level);
}

inline void Disp::_wrapperSetRelays(const void *context, const ActuatorDirection *actuator, const bool *led, const bool *heater, const bool *pump)
{
    Disp* obj = (Disp*)context;
    return obj->_setRelays(actuator, led, heater, pump);
}

Disp::Disp(unsigned char cs, unsigned char rst, unsigned char dc) : u8g2(U8G2_R0, cs, dc, rst) {}

void Disp::Init(DHTSensor* dhtIn, DHTSensor* dhtOut, SoilSensor* soil1, SoilSensor* soil2, 
    SoilSensor* soil3, WaterLevelSensor* water, Light* light, Relays* relays, Enkoder *enkoder, virtuabotixRTC *rtc, Processor* processor)
{
    _dht_in = dhtIn;
    _dht_out = dhtOut;
    _soil[0] = soil1;
    _soil[1] = soil2;
    _soil[2] = soil3;
    _water = water;
    _light = light;
    _relays = relays;
    _enkoder = enkoder;
    _rtc = rtc;
    _processor = processor;
    u8g2.begin();
    u8g2.setContrast(_brightness);
    _enkoder->addButtonCallback(this, _wrapperEncPressed);
    _enkoder->addTurnedCallback(this, _wrapperEncTurned);
    _dht_in->addCallback(this, _wrapperSetDHTIn);
    _dht_out->addCallback(this, _wrapperSetDHTOut);
    _soil[0]->addCallback(this, _wrapperSetSoil);
    _soil[1]->addCallback(this, _wrapperSetSoil);
    _soil[2]->addCallback(this, _wrapperSetSoil);
    _water->addCallback(this, _wrapperSetWater);
    _light->addCallback(this, _wrapperSetLight);
    _relays->addCallback(this, _wrapperSetRelays);
    Serial.println("Display initialized.");
}

void Disp::update()
{
    if(millis() - _blanking_start >= _blanking_time * 60 * 1000) {
        u8g2.setContrast(_blanking_brightness);
    }

    unsigned long requiredDelay = (_turned) ? (1000UL * _back_to_switching_time) : (1000UL * _screan_switch_time);

    if (!_curentItem && (millis() - _last_switch_time > requiredDelay))
    {
        _turned = false;
        _screen_num++;
        if (_screen_num > 5)
            _screen_num = 1;
        _showScreen();
    }
    else if (_curentItem && millis() - _last_switch_time >= (1000UL * _back_to_switching_time))
    {
        _screen_num = 1;
        _curentItem = nullptr;
        _showScreen();
    }
}

inline unsigned char Disp::brightness(const unsigned char *val)
{
    if (val) _brightness = *val;
    return _brightness;
}

inline unsigned char Disp::blankingBrightness(const unsigned char *val)
{
    if (val) _blanking_brightness = *val;
    return _blanking_brightness;
}

inline unsigned char Disp::blankingTime(const unsigned char *time)
{
    if (time) _blanking_time = *time;
    return _blanking_time;
}

inline unsigned char Disp::screanSwitchTime(const unsigned char *time)
{
    if (time) _screan_switch_time = *time;
    return _screan_switch_time;
}

unsigned char Disp::wrapperBrightness(const void *context, const unsigned char *val)
{
    Disp* obj = (Disp*)context;
    return obj->brightness(val);
}

unsigned char Disp::wrapperBlankingBrightness(const void *context, const unsigned char *val)
{
    Disp* obj = (Disp*)context;
    return obj->blankingBrightness(val);
}

unsigned char Disp::wrapperBlankingTime(const void *context, const unsigned char *time)
{
    Disp* obj = (Disp*)context;
    return obj->blankingTime(time);
}

unsigned char Disp::wrapperScreanSwitchTime(const void *context, const unsigned char *time)
{
    Disp* obj = (Disp*)context;
    return obj->screanSwitchTime(time);
}
