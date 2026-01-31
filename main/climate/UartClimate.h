#pragma once

#include <memory>

#include "ClimateUART.h"

#include "climate/IClimate.h"
class UartClimate : public IClimate
{
public:
    UartClimate(std::shared_ptr<climate_uart::ClimateInterface> interface);
    virtual ~UartClimate() = default;
    void lock() override;
    void unlock() override;

    bool setup() override;
    bool refresh() override;

    float getTargetTemperature() const override;
    bool setTargetTemperature(float temperature) override;

    HAClimateMode getMode() const override;
    bool setMode(HAClimateMode mode) override;

    HAClimateFanMode getFanMode() const override;
    bool setFanMode(HAClimateFanMode fanMode) override;

    HAClimateVaneMode getVaneMode() const override;
    bool setVaneMode(HAClimateVaneMode vaneMode) override;

protected:
    std::shared_ptr<climate_uart::ClimateInterface> interface;
    float room_temp;
    float target_temp;
    HAClimateMode mode;
    HAClimateFanMode fan_mode;
    HAClimateAction action;
    HAClimateVaneMode vane_mode;
    bool is_locked;

    bool updateState();

    bool HAMqttToUart(HAClimateFanMode fanMode,
                      HAClimateMode mode,
                      HAClimateVaneMode vaneMode,
                      float targetTemp,
                      climate_uart::ClimateSettings &outParams);

    bool UartToHAMqtt(const climate_uart::ClimateSettings &inParams,
                      HAClimateFanMode &outFanMode,
                      HAClimateMode &outMode,
                      HAClimateVaneMode &outVaneMode,
                      float &outTargetTemp);
};