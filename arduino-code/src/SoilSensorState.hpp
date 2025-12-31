#pragma once

enum SoilSensorState{
    UNINITIALIZED = 0,
    SOIL_E_WET = 210,
    SOIL_WET = 250,
    SOIL_MOIST = 290,
    SOIL_DRY = 330,
    SOIL_E_DRY = 370
};