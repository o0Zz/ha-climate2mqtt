#pragma once

typedef enum {
    HAClimateMode_Off = 0,
    HAClimateMode_Heat = 1,
    HAClimateMode_Cool = 2,
    HAClimateMode_Auto = 3,
    HAClimateMode_Dry = 4,
    HAClimateMode_Fan_Only = 5
} HAClimateMode;

typedef enum {
    HAClimateFanMode_Auto = 0,
    HAClimateFanMode_Diffuse = 1, //Quiet
    HAClimateFanMode_Low = 2,
    HAClimateFanMode_Medium = 3,
    HAClimateFanMode_High = 4
} HAClimateFanMode;

typedef enum {
    HAClimateVaneMode_Auto = 0,
    HAClimateVaneMode_1 = 1,
    HAClimateVaneMode_2 = 2,
    HAClimateVaneMode_3 = 3,
    HAClimateVaneMode_4 = 4,
    HAClimateVaneMode_5 = 5,
    HAClimateVaneMode_Swing = 6
} HAClimateVaneMode;

typedef enum {
    HAClimateAction_Idle = 0,
    HAClimateAction_Heating = 1,
    HAClimateAction_Cooling = 2,
    HAClimateAction_Drying = 3,
    HAClimateAction_Fan_Only = 4
} HAClimateAction;

class IClimate
{
public:
    virtual ~IClimate() = default;

    virtual bool setup() = 0;
    virtual bool refresh() = 0;

    virtual void lock() = 0;
    virtual void unlock() = 0;

    virtual float getTargetTemperature() const = 0;
    virtual bool setTargetTemperature(float temperature) = 0;

    virtual HAClimateMode getMode() const = 0;
    virtual bool setMode(HAClimateMode mode) = 0;

    virtual HAClimateFanMode getFanMode() const = 0;
    virtual bool setFanMode(HAClimateFanMode fanMode) = 0;

    virtual HAClimateVaneMode getVaneMode() const = 0;
    virtual bool setVaneMode(HAClimateVaneMode swingMode) = 0;
};