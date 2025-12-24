#pragma once

#include <Arduino.h>

// Maksymalny czas bez ruchu enkodera po którym pozycja jest zerowana (ms)
#define ENKODER_TIMEOUT 5000 //ms
// Opóźnienie eliminujące drgania styków przycisku (ms)
#define BUTTON_DEBOUNCE_DELAY 200 //ms

// Wskaźnik na funkcję wywoływaną bez argumentów (np. naciśnięcie przycisku)
using EnkoderCallback = void (*)();
// Wskaźnik na funkcję wywoływaną z argumentem pozycji enkodera
using EnkoderCallbackWithArg = void (*)(int);

/**
 * Klasa obsługująca enkoder (rotary encoder) z przyciskiem.
 *
 * Zapewnia:
 * - obsługę przerwań dla kanałów A/B (CLK/DT) oraz przycisku (SW),
 * - automatyczne usuwanie drgań styków przycisku (debounce),
 * - wykrywanie kierunku obrotu i aktualnej pozycjii,
 * - możliwość rejestrowania callbacków dla naciśnięcia przycisku oraz obrotu.
 */
class Enkoder
{
private:
    /** Wskaźnik na jedyną instancję używaną przez statyczne ISRy */
    static Enkoder* _instance;
    /** Callback wywoływany przy naciśnięciu przycisku */
    EnkoderCallback _btPresed = nullptr;
    /** Callback wywoływany po obrocie enkodera, z aktualną pozycją */
    EnkoderCallbackWithArg _encTurned = nullptr;
    /** Pin sygnału CLK (kanał A) */
    unsigned char _pin_CLK;
    /** Pin sygnału DT (kanał B) */
    unsigned char _pin_DT;
    /** Pin przycisku SW */
    unsigned char _pin_SW;
    /** Bieżąca pozycja enkodera (dodatnia/ujemna) */
    volatile int _position;
    /** Znacznik czasu ostatniej zmiany pozycji enkodera (ms) */
    volatile unsigned long _lastEncChange = 0;
    /** Znacznik czasu ostatniego wciśnięcia przycisku (ms) */
    volatile unsigned long _lastBtnPress = 0;
    /** Flaga: pozycja uległa zmianie od ostatniej pętli */
    volatile bool _encChanged = false;
    /** Flaga: przycisk został naciśnięty od ostatniej pętli */
    volatile bool _btnChanged = false;

    // Instance methods called from static ISRs
    /**
     * Obsługa naciśnięcia przycisku z uwzględnieniem debounce.
     * Ustawia flagę zmiany oraz wywołuje zarejestrowany callback.
     */
    void doButton()
    {
        if(millis() - _lastBtnPress >= BUTTON_DEBOUNCE_DELAY || _lastBtnPress == 0)
        {
            _lastBtnPress = millis();
            _btnChanged = true;
            if (_btPresed) _btPresed();
        }
    }

    /**
     * Obsługa obrotu enkodera: wykrycie kierunku (CLK vs DT),
     * aktualizacja pozycji, reset po bezczynności oraz wywołanie callbacku.
     */
    void doEncoder()
    {
        if(millis() - _lastEncChange >= ENKODER_TIMEOUT) _position = 0;
        if (digitalRead(_pin_CLK) != digitalRead(_pin_DT)) {
            _position++; // Obrót w prawo
        } else {
            _position--; // Obrót w lewo
        }
        _lastEncChange = millis();
        _encChanged = true;
        if (_encTurned) _encTurned(_position);
    }

    // Static wrappers used as ISRs
    /** Statyczny wrapper ISR dla kanałów enkodera (CLK/DT) */
    static void isrEncoder()
    {
        if (_instance) _instance->doEncoder();
    }

    /** Statyczny wrapper ISR dla przycisku SW */
    static void isrButton()
    {
        if (_instance) _instance->doButton();
    }

public:
    /**
     * Tworzy obiekt enkodera i zapamiętuje piny.
     * Przerwania konfiguruje dopiero metoda Init.
     *
     * @param pin_CLK Pin kanału A (CLK)
     * @param pin_DT  Pin kanału B (DT)
     * @param pin_SW  Pin przycisku (SW)
     */
    Enkoder(unsigned char pin_CLK, unsigned char pin_DT, unsigned char pin_SW)
    {
        _instance = this;

        _pin_CLK = pin_CLK;
        _pin_DT = pin_DT;
        _pin_SW = pin_SW;
    }

    /**
     * Niszczy obiekt i odłącza przerwania.
     */
    ~Enkoder()
    {
        detachInterrupt(digitalPinToInterrupt(_pin_CLK));
        detachInterrupt(digitalPinToInterrupt(_pin_SW));
        if (_instance == this) _instance = nullptr;
    }

    /**
     * Inicjalizuje enkoder: ustawia tryby pinów, rejestruje
     * callbacki i podłącza przerwania.
     *
     * @param btPresed  Callback dla naciśnięcia przycisku
     * @param encTurned Callback dla obrotu (otrzymuje bieżącą pozycję)
     */
    void Init(EnkoderCallback btPresed, EnkoderCallbackWithArg encTurned)
    {
        _btPresed = btPresed;
        _encTurned = encTurned;

        pinMode(_pin_CLK, INPUT_PULLUP);
        pinMode(_pin_DT, INPUT_PULLUP);
        pinMode(_pin_SW, INPUT_PULLUP);

        attachInterrupt(digitalPinToInterrupt(_pin_CLK), Enkoder::isrEncoder, CHANGE);
        attachInterrupt(digitalPinToInterrupt(_pin_SW), Enkoder::isrButton, FALLING);
    }

    /**
     * Zwraca bieżącą pozycję enkodera. Po okresie bezczynności (> ENKODER_TIMEOUT)
     * pozycja jest zerowana.
     * @return Aktualna pozycja enkodera.
     */
    int getEncPos()
    {
        if(millis() - _lastEncChange > ENKODER_TIMEOUT) _position = 0;
        return _position;
    }

    /**
     * Metoda do wywoływania w pętli głównej (loop). Odczytuje
     * i resetuje wewnętrzne flagi, a następnie wywołuje odpowiednie callbacki.
     */
    void loop()
    {
        if(_encChanged)
        {
            _encChanged = false;
            if (_encTurned) _encTurned(getEncPos());
        }
        if(_btnChanged)
        {
            _btnChanged = false;
            if (_btPresed) _btPresed();
        }
    }
};

// Definition of the static instance pointer
Enkoder* Enkoder::_instance = nullptr;
