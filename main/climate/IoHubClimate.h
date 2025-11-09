#pragma once

#include "climate/IClimate.h"
#include "heatpump/iohub_heatpump_common.h"

class IoHubClimate : public IClimate 
{
public:
    IoHubClimate();
    virtual ~IoHubClimate() = default;

    void lock() override;
    void unlock() override;

    float getTargetTemperature() const override;
    bool setTargetTemperature(float temperature) override;

    HAClimateMode getMode() const override;
    bool setMode(HAClimateMode mode) override;

    HAClimateFanMode getFanMode() const override;
    bool setFanMode(HAClimateFanMode fanMode) override;

    HAClimateVaneMode getVaneMode() const override;
    bool setVaneMode(HAClimateVaneMode vaneMode) override;

protected:
    float room_temp;
    float target_temp;
    HAClimateMode mode;
    HAClimateFanMode fan_mode;
    HAClimateAction action;
    HAClimateVaneMode vane_mode;
    bool is_locked;

    virtual bool updateState() = 0;

    void HAMqttToIoHub(HAClimateFanMode fanMode,
                        HAClimateMode mode,
                        HAClimateVaneMode vaneMode,
                        float targetTemp,
                        IoHubHeatpumpSettings &outParams);

    void IoHubToHAMqtt(const IoHubHeatpumpSettings &inParams,
                        HAClimateFanMode &outFanMode, 
                        HAClimateMode &outMode,
                        HAClimateVaneMode &outVaneMode,
                        float &outTargetTemp);
};