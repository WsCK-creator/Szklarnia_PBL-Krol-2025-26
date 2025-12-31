#pragma once

#define ITEM_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

enum MenuID {
    ID_NONE = 0, 

    // --- Manual Mode Items ---
    //ACTUATOR_DIRECTION,
    ACTUATOR,
    LED,
    PUMP,
    HEATER,
    //VARIABLE_VIEW,

    // --- Relay Settings ---
    SWITCHING_DELAY,
    SWITCHING_OFF_DELAY,

    // --- Actuator Settings ---
    /*ACTUATOR_TEMP_ON,
    ACTUATOR_TEMP_OFF,
    ACTUATOR_HUM_ON,
    ACTUATOR_HUM_OFF,

    // --- Heater Settings ---
    HEATER_TEMP,
    HEATER_HYS_TEMP,*/

    // --- HEAT AND HUM SETTINGS ---
    TEMP_SETPOINT,
    TEMP_HYS,
    HUM_SETPOINT,
    HUM_HYS,

    // --- Pump Settings ---
    PUMP_SENSOR_COUNT,
    PUMP_SETPOINT,
    PUMP_RUN_TIME,
    PUMP_RUN_INTERVAL,

    // --- LED Settings --- //TODO
    LED_THRESHOLD,
    LED_HYS,
    LED_RUN_TIME,
    LED_RUN_INTERVAL,

    // --- Display Settings ---
    DISPLAY_BRIGHTNESS, 
    DISPLAY_SAVER_BRIGHTNESS,
    DISPLAY_SAVER_TIME,
    DISPLAY_SWITCH_TIME,

    // --- Sensors Settings ---
    SENSORS_SOIL1_DELAY,
    SENSORS_SOIL1_HYS,
    SENSORS_SOIL2_DELAY,
    SENSORS_SOIL2_HYS,
    SENSORS_SOIL3_DELAY,
    SENSORS_SOIL3_HYS,
    SENSORS_DHT_IN,
    SENSORS_DHT_OUT,
    SENSORS_WATER,
    SENSORS_PHOTO, //TODO

    // --- System ---
    //MENU_ID_COUNT 
};

struct MenuItem {
    const MenuItem* parent;
    const char* header;
    const char* menuName;
    const MenuItem* const* items;
    const MenuID id;                
    unsigned char count;    
};

// --- FORWARD DECLARATIONS ---
extern const MenuItem mainMenu;
extern const MenuItem itemManualMode;
extern const MenuItem itemSettings;
extern const MenuItem itemRelaySettings;
//extern const MenuItem itemActuatorSettings;
//extern const MenuItem itemHeaterSettings;
extern const MenuItem itemTempAndHumSettings;
extern const MenuItem itemPumpSettings;
extern const MenuItem itemLEDSettings;
extern const MenuItem itemDisplaySettings;
extern const MenuItem itemSensorsSettings;
extern const MenuItem itemSensorsSoil1;
extern const MenuItem itemSensorsSoil2;
extern const MenuItem itemSensorsSoil3;

const MenuItem itemBack = { nullptr, "Powrot", nullptr, nullptr, ID_NONE, 0};

// --- MANUAL MODE ---
//const MenuItem itemActuatorDirection = {&itemManualMode, "KIERUNEK", "Kierunek Klapy", nullptr, ACTUATOR_DIRECTION, 0};
const MenuItem itemActuator          = {&itemManualMode, "KLAPA", "Klapa Praca",       nullptr, ACTUATOR, 0};
const MenuItem itemLED               = {&itemManualMode, "LED", "LED Praca",           nullptr, LED, 0};
const MenuItem itemPump              = {&itemManualMode, "POMPA", "Pompa Praca",       nullptr, PUMP, 0};
const MenuItem itemHeater            = {&itemManualMode, "GRZALKA", "Grzalka Praca",   nullptr, HEATER, 0};
//const MenuItem itemVariableView    = {&itemManualMode, "DANE", "Podglad Danych",    nullptr, VARIABLE_VIEW, 0};

const MenuItem* const manualModeItems[] = {
    &itemBack,
    //&itemActuatorDirection,
    &itemActuator,
    &itemLED,
    &itemPump,
    &itemHeater,
    //&itemVariableView
};

const MenuItem itemManualMode = {&mainMenu, "TRYB RECZNY", "Tryb reczny", manualModeItems, ID_NONE, ITEM_COUNT(manualModeItems)};

// --- ACTUATOR SETTINGS ---
/*const MenuItem itemActuatorTempOn  = {&itemActuatorSettings, "TEMP. OTW.", "Temp. Otwarcia",    nullptr, ACTUATOR_TEMP_ON, 0};
const MenuItem itemActuatorTempOff = {&itemActuatorSettings, "TEMP. ZAM.", "Temp. Zamkniecia",  nullptr, ACTUATOR_TEMP_OFF, 0};
const MenuItem itemActuatorHumOn   = {&itemActuatorSettings, "WIL. OTW.", "Wilgoc Otwarcia",    nullptr, ACTUATOR_HUM_ON, 0};
const MenuItem itemActuatorHumOff  = {&itemActuatorSettings, "WIL. ZAM.", "Wilgoc Zamkniecia",  nullptr, ACTUATOR_HUM_OFF, 0};
*/

// --- Relay Settings ---
const MenuItem itemSwitchingDelay = {&itemRelaySettings, "T PRZEL. STY.", "Czas Przel. Styku", nullptr, SWITCHING_DELAY, 0};
const MenuItem itemSwitchingOffDelay = {&itemRelaySettings, "T WYL. STY.", "Czas Wyl. Styku", nullptr, SWITCHING_OFF_DELAY, 0};

const MenuItem* const relaySettingsItems[] = {
    &itemBack,
    &itemSwitchingDelay,
    &itemSwitchingOffDelay
    /*&itemActuatorTempOn,
    &itemActuatorTempOff,
    &itemActuatorHumOn,
    &itemActuatorHumOff*/
};

const MenuItem itemRelaySettings = {&itemSettings, "STYKI", "Styki", relaySettingsItems, ID_NONE, ITEM_COUNT(relaySettingsItems)};

// --- HEAT AND HUM SETTINGS ---
const MenuItem itemTempSetpoint = {&itemTempAndHumSettings, "TEMP. ZAD.", "Temp. Zadana", nullptr, TEMP_SETPOINT, 0};
const MenuItem itemTempHys = {&itemTempAndHumSettings, "HIS. TEMP.", "Histereza Temp.", nullptr, TEMP_HYS, 0};
const MenuItem itemHumSetpoint = {&itemTempAndHumSettings, "WIL. ZAD.", "Wil. Zadana", nullptr, HUM_SETPOINT, 0};
const MenuItem itemHumHys = {&itemTempAndHumSettings, "HIS. WIL.", "Histereza Wil.", nullptr, HUM_HYS, 0};

const MenuItem* const tempAndHumItems[] = {
    &itemBack,
    &itemTempSetpoint,
    &itemTempHys,
    &itemHumSetpoint,
    &itemHumHys
};

const MenuItem itemTempAndHumSettings = {&itemSettings, "TEMP. i WIL.", "Temp. i Wil.", tempAndHumItems, ID_NONE, ITEM_COUNT(tempAndHumItems)};

// --- HEATER SETTINGS ---
/*const MenuItem itemHeaterTemp    = {&itemHeaterSettings, "TEMP. ZAD.", "Temp.zadana",     nullptr, HEATER_TEMP, 0};
const MenuItem itemHeaterHysTemp = {&itemHeaterSettings, "HIS. TEMP.", "Histereza Temp.", nullptr, HEATER_HYS_TEMP, 0};

const MenuItem* const heaterSettingsItems[] = {
    &itemBack,
    &itemHeaterTemp,
    &itemHeaterHysTemp
};

const MenuItem itemHeaterSettings = {&itemSettings, "GRZALKA", "Grzalka", heaterSettingsItems, ID_NONE, ITEM_COUNT(heaterSettingsItems)};*/

// --- PUMP SETTINGS ---
const MenuItem itemPumpSensorCount = {&itemPumpSettings, "IL. ZG. CZUJ.", "Il. Zgodnych Czuj.", nullptr, PUMP_SENSOR_COUNT, 0};
const MenuItem itemPumpSetpoint   = {&itemPumpSettings, "PROG ZAL.", "Prog Zalaczenia",        nullptr, PUMP_SETPOINT, 0};
const MenuItem itemPumpRunTime        = {&itemPumpSettings, "CZAS PODL.", "Czas Podlewania",       nullptr, PUMP_RUN_TIME, 0};
const MenuItem itemPumpRunInterval    = {&itemPumpSettings, "INTERWAL", "Interwal Podl.",          nullptr, PUMP_RUN_INTERVAL, 0};

const MenuItem* const pumpSettingsItems[] = {
    &itemBack,
    &itemPumpSensorCount,
    &itemPumpSetpoint,
    &itemPumpRunTime,
    &itemPumpRunInterval
};

const MenuItem itemPumpSettings = {&itemSettings, "POMPA", "Pompa", pumpSettingsItems, ID_NONE, ITEM_COUNT(pumpSettingsItems)};

// --- LED SETTINGS ---
const MenuItem itemLEDThreshold = {&itemLEDSettings, "PROG SWIATLA", "Prog Swiatla",       nullptr, LED_THRESHOLD, 0};
const MenuItem itemLEDHys       = {&itemLEDSettings, "HIS. SWIATLA", "Histereza Swiatla",  nullptr, LED_HYS, 0};
const MenuItem itemLEDRunTime      = {&itemLEDSettings, "CZAS SWIECENIA", "Czas Swiecenia",   nullptr, LED_RUN_TIME, 0};
const MenuItem itemLEDRunInterval  = {&itemLEDSettings, "INTERWAL", "Interwal Swiecenia",     nullptr, LED_RUN_INTERVAL, 0};

const MenuItem* const LEDSettingsItems[] = {
    &itemBack,
    &itemLEDThreshold,
    &itemLEDHys,
    &itemLEDRunTime,
    &itemLEDRunInterval
};

const MenuItem itemLEDSettings = {&itemSettings, "LED", "LED", LEDSettingsItems, ID_NONE, ITEM_COUNT(LEDSettingsItems)};

// --- DISPLAY SETTINGS ---
const MenuItem itemDisplayBrightness      = {&itemDisplaySettings, "JASNOSC", "Jasnosc",            nullptr, DISPLAY_BRIGHTNESS, 0};
const MenuItem itemDisplaySaverBrightness = {&itemDisplaySettings, "JASNOSC WYG.", "Jasnosc Wyg.",  nullptr, DISPLAY_SAVER_BRIGHTNESS, 0};
const MenuItem itemDisplaySaverTime       = {&itemDisplaySettings, "CZAS WYG.", "Czas Wygaszacza",  nullptr, DISPLAY_SAVER_TIME, 0};
const MenuItem itemDisplaySwitchTime      = {&itemDisplaySettings, "CZAS ZM. SC.", "Czas Zmiany Scen", nullptr, DISPLAY_SWITCH_TIME, 0};

const MenuItem* const displaySettingsItems[] = {
    &itemBack,
    &itemDisplayBrightness,
    &itemDisplaySaverBrightness,
    &itemDisplaySaverTime,
    &itemDisplaySwitchTime
};

const MenuItem itemDisplaySettings = {&itemSettings, "OLED", "Wyswietlacz", displaySettingsItems, ID_NONE, ITEM_COUNT(displaySettingsItems)};

// --- SENSORS SETTINGS ---
/*const MenuItem itemSensorsSoil1  = {&itemSensorsSettings, "SOIL1", "Soil1",   nullptr, SENSORS_SOIL1, 0};
const MenuItem itemSensorsSoil2  = {&itemSensorsSettings, "SOIL2", "Soil2",   nullptr, SENSORS_SOIL2, 0};
const MenuItem itemSensorsSoil3  = {&itemSensorsSettings, "SOIL3", "Soil3",   nullptr, SENSORS_SOIL3, 0};*/
const MenuItem itemSensorsSoil1Delay = {&itemSensorsSoil1, "S1 DELAY", "Soil1 Delay",   nullptr, SENSORS_SOIL1_DELAY, 0};
const MenuItem itemSensorsSoil1Hys = {&itemSensorsSoil1, "S1 HYS", "Soil1 Hys",   nullptr, SENSORS_SOIL1_HYS, 0};

const MenuItem* const sensorsSoil1Items[] = {
    &itemBack,
    &itemSensorsSoil1Delay,
    &itemSensorsSoil1Hys
};

const MenuItem itemSensorsSoil2Delay = {&itemSensorsSoil2, "S2 DELAY", "Soil2 Delay",   nullptr, SENSORS_SOIL2_DELAY, 0};
const MenuItem itemSensorsSoil2Hys = {&itemSensorsSoil2, "S2 HYS", "Soil2 Hys",   nullptr, SENSORS_SOIL2_HYS, 0};

const MenuItem* const sensorsSoil2Items[] = {
    &itemBack,
    &itemSensorsSoil2Delay,
    &itemSensorsSoil2Hys
};

const MenuItem itemSensorsSoil3Delay = {&itemSensorsSoil3, "S3 DELAY", "Soil3 Delay",   nullptr, SENSORS_SOIL3_DELAY, 0};
const MenuItem itemSensorsSoil3Hys = {&itemSensorsSoil3, "S3 HYS", "Soil3 Hys",   nullptr, SENSORS_SOIL3_HYS, 0};

const MenuItem* const sensorsSoil3Items[] = {
    &itemBack,
    &itemSensorsSoil3Delay,
    &itemSensorsSoil3Hys
};

const MenuItem itemSensorsSoil1  = {&itemSensorsSettings, "SOIL1", "Soil1", sensorsSoil1Items, ID_NONE, ITEM_COUNT(sensorsSoil1Items)};
const MenuItem itemSensorsSoil2  = {&itemSensorsSettings, "SOIL2", "Soil2", sensorsSoil2Items, ID_NONE, ITEM_COUNT(sensorsSoil2Items)};
const MenuItem itemSensorsSoil3  = {&itemSensorsSettings, "SOIL3", "Soil3", sensorsSoil3Items, ID_NONE, ITEM_COUNT(sensorsSoil3Items)};
const MenuItem itemSensorsDHTIn  = {&itemSensorsSettings, "T IN DEL", "T In R Delay",   nullptr, SENSORS_DHT_IN, 0};
const MenuItem itemSensorsDHTOut = {&itemSensorsSettings, "T OUT DEL", "T Out R Delay", nullptr, SENSORS_DHT_OUT, 0};
const MenuItem itemSensorsWater  = {&itemSensorsSettings, "WATER DEL", "Water R Delay",   nullptr, SENSORS_WATER, 0};
const MenuItem itemSensorsPhoto  = {&itemSensorsSettings, "PHOTO DEL", "Photo R Delay",   nullptr, SENSORS_PHOTO, 0};

const MenuItem* const sensorsSettingsItems[] = {
    &itemBack,
    &itemSensorsSoil1,
    &itemSensorsSoil2,
    &itemSensorsSoil3,
    &itemSensorsDHTIn,
    &itemSensorsDHTOut,
    &itemSensorsWater,
    &itemSensorsPhoto
};

const MenuItem itemSensorsSettings = {&itemSettings, "CZUJNIKI", "Zwloka Czujnikow", sensorsSettingsItems, ID_NONE, ITEM_COUNT(sensorsSettingsItems)};

// --- MAIN STRUCTURE ---
const MenuItem* const settingsItems[] = {
    &itemBack,
    /*&itemActuatorSettings,
    &itemHeaterSettings,*/
    &itemTempAndHumSettings,
    &itemPumpSettings,
    &itemLEDSettings,
    &itemDisplaySettings,
    &itemSensorsSettings
};

const MenuItem itemSettings = {&mainMenu, "USTAWIENIA", "Ustawienia", settingsItems, ID_NONE, ITEM_COUNT(settingsItems)};

const MenuItem* const mainItems[] = {
    &itemBack,
    &itemManualMode,
    &itemSettings
};

const MenuItem mainMenu = { 
    nullptr,
    "GLOWNE MENU", 
    nullptr, 
    mainItems,
    ID_NONE, 
    ITEM_COUNT(mainItems)
};