#pragma once

#include "climate/IClimate.h"

class EmptyClimate : public IClimate 
{
public:
    EmptyClimate() = default;
    ~EmptyClimate() override = default;

    bool refresh() override { return true; }

    float getTargetTemperature() const override { return 0.0f; }
    bool setTargetTemperature(float temperature) override { return true; }

    HAClimateMode getMode() const override { return HAClimateMode_Off; }
    bool setMode(HAClimateMode mode) override { return true; }

    HAClimateFanMode getFanMode() const override { return HAClimateFanMode_Auto; }
    bool setFanMode(HAClimateFanMode fanMode) override { return true; }

    HAClimateVaneMode getVaneMode() const override { return HAClimateVaneMode_Auto; }
    bool setVaneMode(HAClimateVaneMode swingMode) override { return true; }
};