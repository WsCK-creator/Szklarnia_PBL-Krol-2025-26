#pragma once

using CbFloat  = float (*)(const void* context, const float *val);
using CbUChar  = unsigned char (*)(const void* context, const unsigned char *val);
using CbUShort = unsigned short (*)(const void* context, const unsigned short *val);
using CbBool   = bool (*)(const void* context, const bool *val);
using CbEnum   = short (*)(const void* context, const short *val);

enum DataType
{
    TYPE_UCHAR,
    TYPE_USHORT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_ENUM
};

struct ConfigUChar
{
    const CbUChar callback;
    const unsigned char minVal;
    const unsigned char maxVal;
    const unsigned char step;
    const char *unit;
};

struct ConfigUShort
{
    const CbUShort callback;
    const unsigned short minVal;
    const unsigned short maxVal;
    const unsigned short step;
    const char *unit;
};

struct ConfigFloat
{
    const CbFloat callback;
    const float minVal;
    const float maxVal;
    const float step;
    const char *unit;
};

struct ConfigBool
{
    const CbBool callback;
    const char *txtOn;
    const char *txtOff;
};

struct ConfigEnum
{
    const CbEnum callback;
    const char **options;
    const char maxVal;
};

struct DataConfig
{
    const DataType type;
    union{
        const ConfigUChar *confUChar;
        const ConfigUShort *confUShort;
        const ConfigFloat *confFloat;
        const ConfigBool *confBool;
        const ConfigEnum *confEnum;
    } ptr;
};