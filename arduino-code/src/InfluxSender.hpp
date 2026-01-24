#pragma once

#include <Arduino.h>
#include "WiFiEsp.h"

#include "DHTSensor.hpp"
#include "Light.hpp"
#include "WaterLevelSensor.hpp"
#include "SoilSensor.hpp"

// Definicje stałych nazw pól w InfluxDB
#define DATA_TEMP_IN "temp_in"
#define DATA_TEMP_OUT "temp_out"
#define DATA_HUM_IN "hum_in"
#define DATA_HUM_OUT "hum_out"
#define DATA_WATER_L "water_level"
#define DATA_LIGHT_L "light_level"
#define DATA_SOIL_HUM_1 "soil_hum_1"
#define DATA_SOIL_HUM_2 "soil_hum_2"
#define DATA_SOIL_HUM_3 "soil_hum_3"

class InfluxSender {
private:
    // Konfiguracja sieci i serwera
    const char* _ssid;
    const char* _pass;
    const char* _host;
    int _port;
    const char* _dbName;
    const char* _measurement;
    const char* _version;
    
    // Timer
    unsigned long _lastSendTime;
    unsigned long _logPeriod;

    // Klient WiFi
    WiFiEspClient _client;

    // Dane do wysłania
    float _tempIn;
    float _tempOut;
    float _humIn;
    float _humOut;
    int _soilHum1;
    int _soilHum2;
    int _soilHum3;
    int _waterLevel;
    int _lightLevel;

    // Wskaźniki na sensory
    DHTSensor* _dht_in;
    DHTSensor* _dht_out;
    SoilSensor* _soil[3];
    WaterLevelSensor* _water;
    Light* _light;

    // Metody callbacków
    void _onDHTInChanged(const float* temp, const float* hum);
    void _onDHTOutChanged(const float* temp, const float* hum);
    void _onLightChanged(const unsigned char* level);
    void _onWaterChanged(const unsigned char* level);
    void _onSoilChanged(const unsigned char* id, const SoilSensorState* state);

    // Wrapper callbacki
    static void _wrapperDHTInChanged(const void* context, const float* temp, const float* hum);
    static void _wrapperDHTOutChanged(const void* context, const float* temp, const float* hum);
    static void _wrapperLightChanged(const void* context, const unsigned char* level);
    static void _wrapperWaterChanged(const void* context, const unsigned char* level);
    static void _wrapperSoilChanged(const void* context, const unsigned char* id, const SoilSensorState* state);

    // Deklaracja metody prywatnej
    void sendDataToDB();

public:
    // Deklaracja Konstruktora
    InfluxSender(const char* ssid, const char* pass, const char* host, int port, const char* db, const char* measurement, unsigned long logPeriodMs, const char* version);

    // Deklaracje metod publicznych
    void Init(Stream* wifiSerial, DHTSensor* dhtIn, DHTSensor* dhtOut, SoilSensor* soil1, 
             SoilSensor* soil2, SoilSensor* soil3, WaterLevelSensor* water, Light* light);
    void Update();
};

// ================================================================
// IMPLEMENTACJA METOD (KOD FUNKCJI)
// ================================================================

// Konstruktor
InfluxSender::InfluxSender(const char* ssid, const char* pass, const char* host, int port, const char* db, const char* measurement, unsigned long logPeriodMs, const char* version){
    _ssid = ssid;
    _pass = pass;
    _host = host;
    _port = port;
    _dbName = db;
    _measurement = measurement;
    _logPeriod = logPeriodMs;
    _lastSendTime = 0;
    _version = version;
    
    // Zerowanie zmiennych
    _tempIn = 0.0; _tempOut = 0.0; _humIn = 0.0; _humOut = 0.0;
    _soilHum1 = 0; _soilHum2 = 0; _soilHum3 = 0;
    _waterLevel = 0; _lightLevel = 0;

    // Zerowanie wskaźników
    _dht_in = nullptr;
    _dht_out = nullptr;
    _soil[0] = nullptr;
    _soil[1] = nullptr;
    _soil[2] = nullptr;
    _water = nullptr;
    _light = nullptr;
}

// Inicjalizacja
void InfluxSender::Init(Stream* wifiSerial, DHTSensor* dhtIn, DHTSensor* dhtOut, SoilSensor* soil1, 
                         SoilSensor* soil2, SoilSensor* soil3, WaterLevelSensor* water, Light* light) {
    // Przechowywanie wskaźników na sensory
    _dht_in = dhtIn;
    _dht_out = dhtOut;
    _soil[0] = soil1;
    _soil[1] = soil2;
    _soil[2] = soil3;
    _water = water;
    _light = light;

    // Rejestracja callbacków
    _dht_in->addCallback(this, _wrapperDHTInChanged);
    _dht_out->addCallback(this, _wrapperDHTOutChanged);
    _soil[0]->addCallback(this, _wrapperSoilChanged);
    _soil[1]->addCallback(this, _wrapperSoilChanged);
    _soil[2]->addCallback(this, _wrapperSoilChanged);
    _water->addCallback(this, _wrapperWaterChanged);
    _light->addCallback(this, _wrapperLightChanged);

    // Inicjalizacja modułu WiFiEsp na wskazanym porcie Serial
    WiFi.init(wifiSerial);

    // Sprawdzenie obecności modułu
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println(F("[InfluxSender] BLAD: Modul WiFi nie wykryty!"));
        while (true); // Stop
    }

    // Łączenie z WiFi
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(F("[InfluxSender] Laczenie z WiFi: "));
        Serial.println(_ssid);
        WiFi.begin(_ssid, _pass);
    }

    Serial.println(F("[InfluxSender] Polaczono z WiFi!"));
    Serial.print(F("IP: "));
    Serial.println(WiFi.localIP());
    Serial.println("InfluxSender initialized");
}

// Update (główna pętla)
void InfluxSender::Update() {
    if (millis() - _lastSendTime > _logPeriod) {
        _lastSendTime = millis();
        sendDataToDB();
    }
}

// Metoda prywatna wysyłająca dane
void InfluxSender::sendDataToDB() {
    Serial.println(F("[InfluxSender] Rozpoczynam wysylanie danych..."));

    if (_client.connect(_host, _port)) {
        Serial.println(F("[InfluxSender] Polaczono z serwerem!"));

        // 1. Budowanie Payload (Line Protocol)
        String payload = "";
        payload += _measurement;
        payload += ",version=";
        payload += _version;
        payload += " ";
        
        payload += DATA_TEMP_IN; payload += "="; payload += String(_tempIn, 2); payload += ",";
        payload += DATA_TEMP_OUT; payload += "="; payload += String(_tempOut, 2); payload += ",";
        payload += DATA_HUM_IN; payload += "="; payload += String(_humIn, 2); payload += ",";
        payload += DATA_HUM_OUT; payload += "="; payload += String(_humOut, 2); payload += ",";
        payload += DATA_WATER_L; payload += "="; payload += String(_waterLevel); payload += ",";
        payload += DATA_LIGHT_L; payload += "="; payload += String(_lightLevel); payload += ",";
        payload += DATA_SOIL_HUM_1; payload += "="; payload += String(_soilHum1); payload += ",";
        payload += DATA_SOIL_HUM_2; payload += "="; payload += String(_soilHum2); payload += ",";
        payload += DATA_SOIL_HUM_3; payload += "="; payload += String(_soilHum3);

        // 2. Wysyłanie nagłówków HTTP
        _client.print("POST /write?db=");
        _client.print(_dbName);
        _client.println("&precision=s HTTP/1.1");
        
        _client.print("Host: ");
        _client.print(_host);
        _client.print(":");
        _client.println(_port);
        
        _client.println("Content-Type: text/plain; charset=utf-8");
        //_client.println("Connection: close"); 
        
        _client.print("Content-Length: ");
        _client.println(payload.length());
        _client.println(); // Pusta linia
        
        // 3. Wysyłanie danych
        _client.print(payload);
        
        Serial.println(F("[InfluxSender] Dane wyslane. Zamykam polaczenie."));
        _client.stop();
    } else {
        Serial.println(F("[InfluxSender] BLAD: Nie udalo sie polaczyc z serwerem InfluxDB"));
    }
}

// ================================================================
// IMPLEMENTACJA CALLBACKÓW
// ================================================================

inline void InfluxSender::_onDHTInChanged(const float* temp, const float* hum) {
    if(temp) _tempIn = *temp;
    if(hum) _humIn = *hum;
}

inline void InfluxSender::_onDHTOutChanged(const float* temp, const float* hum) {
    if(temp) _tempOut = *temp;
    if(hum) _humOut = *hum;
}

inline void InfluxSender::_onLightChanged(const unsigned char* level) {
    if(level) _lightLevel = *level;
}

inline void InfluxSender::_onWaterChanged(const unsigned char* level) {
    if(level) _waterLevel = *level;
}

inline void InfluxSender::_onSoilChanged(const unsigned char* id, const SoilSensorState* state) {
    if(id) {
        switch(*id) {
            case 0: 
                _soilHum1 = *state;
                break;
            case 1: 
                _soilHum2 = *state;
                break;
            case 2: 
                _soilHum3 = *state;
                break;
        }
    }
}

// ================================================================
// WRAPPER CALLBACKI
// ================================================================

void InfluxSender::_wrapperDHTInChanged(const void* context, const float* temp, const float* hum) {
    InfluxSender* obj = (InfluxSender*)context;
    obj->_onDHTInChanged(temp, hum);
}

void InfluxSender::_wrapperDHTOutChanged(const void* context, const float* temp, const float* hum) {
    InfluxSender* obj = (InfluxSender*)context;
    obj->_onDHTOutChanged(temp, hum);
}

void InfluxSender::_wrapperLightChanged(const void* context, const unsigned char* level) {
    InfluxSender* obj = (InfluxSender*)context;
    obj->_onLightChanged(level);
}

void InfluxSender::_wrapperWaterChanged(const void* context, const unsigned char* level) {
    InfluxSender* obj = (InfluxSender*)context;
    obj->_onWaterChanged(level);
}

void InfluxSender::_wrapperSoilChanged(const void* context, const unsigned char* id, const SoilSensorState* state) {
    InfluxSender* obj = (InfluxSender*)context;
    obj->_onSoilChanged(id, state);
}