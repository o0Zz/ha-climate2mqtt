#include "climate/IoHubClimate.h"
#include "esp_log.h"

#define TAG "IoHubClimate"

IoHubClimate::IoHubClimate():
    room_temp(0.0f),
    target_temp(22.0f),
    mode(HAClimateMode_Off),
    fan_mode(HAClimateFanMode_Auto),
    action(HAClimateAction_Idle),
    vane_mode(HAClimateVaneMode_Auto),
    is_locked(false)
{

}    

void IoHubClimate::lock()
{
    is_locked = true;
}

void IoHubClimate::unlock()
{
    is_locked = false;
    updateState();
}

float IoHubClimate::getTargetTemperature() const 
{
    return target_temp;
}

bool IoHubClimate::setTargetTemperature(float temperature) 
{
    target_temp = temperature;

    if (is_locked)
        return true;

    return updateState();
}

HAClimateMode IoHubClimate::getMode() const 
{
    return mode;
}

bool IoHubClimate::setMode(HAClimateMode mode) 
{
    this->mode = mode;

    if (is_locked)
        return true;
        
    return updateState();
}

HAClimateFanMode IoHubClimate::getFanMode() const 
{
    return fan_mode;
}

bool IoHubClimate::setFanMode(HAClimateFanMode fanMode) 
{
    this->fan_mode = fanMode;

    if (is_locked)
        return true;
        
    return updateState();
}


HAClimateVaneMode IoHubClimate::getVaneMode() const
{
    return vane_mode;
}

bool IoHubClimate::setVaneMode(HAClimateVaneMode vaneMode)
{
    this->vane_mode = vaneMode;
    
    if (is_locked)
        return true;

    return updateState();
}

void IoHubClimate::HAMqttToIoHub(HAClimateFanMode fanMode,
                        HAClimateMode mode,
                        HAClimateVaneMode vaneMode,
                        float targetTemp,
                        IoHubHeatpumpSettings &outParams)
{
    switch (fanMode) {
        case HAClimateFanMode_Auto:
            outParams.mFanSpeed = HeatpumpFanSpeed_Auto;
            break;
        case HAClimateFanMode_Low:
            outParams.mFanSpeed = HeatpumpFanSpeed_Low;
            break;
        case HAClimateFanMode_Medium:
            outParams.mFanSpeed = HeatpumpFanSpeed_Med;
            break;
        case HAClimateFanMode_High:
            outParams.mFanSpeed = HeatpumpFanSpeed_High;
            break;
        default:
            outParams.mFanSpeed = HeatpumpFanSpeed_Auto; // Default
            break;
    }
    if (mode == HAClimateMode_Off) {
        outParams.mAction = HeatpumpAction_OFF;
        outParams.mMode = HeatpumpMode_None;
    } else {
        outParams.mAction = HeatpumpAction_ON;
        switch (mode) {
            case HAClimateMode_Heat:
                outParams.mMode = HeatpumpMode_Heat;
                break;
            case HAClimateMode_Cool:
                outParams.mMode = HeatpumpMode_Cold;
                break;
            case HAClimateMode_Dry:
                outParams.mMode = HeatpumpMode_Dry;
                break;
            case HAClimateMode_Fan_Only:
                outParams.mMode = HeatpumpMode_Fan;
                break;
            case HAClimateMode_Auto:
                outParams.mMode = HeatpumpMode_Auto;
                break;
            default:
                outParams.mMode = HeatpumpMode_None;
                break;
        }
    }

    switch (vaneMode) {
        case HAClimateVaneMode_Auto:
            outParams.mVaneMode = HeatpumpVaneMode_Auto;
            break;
        case HAClimateVaneMode_1:
            outParams.mVaneMode = HeatpumpVaneMode_1;
            break;
        case HAClimateVaneMode_2:
            outParams.mVaneMode = HeatpumpVaneMode_2;
            break;
        case HAClimateVaneMode_3:
            outParams.mVaneMode = HeatpumpVaneMode_3;
            break;
        case HAClimateVaneMode_4:
            outParams.mVaneMode = HeatpumpVaneMode_4;
            break;
        case HAClimateVaneMode_5:
            outParams.mVaneMode = HeatpumpVaneMode_5;
            break;
        case HAClimateVaneMode_Swing:
            outParams.mVaneMode = HeatpumpVaneMode_Swing;
            break;
        default:
            outParams.mVaneMode = HeatpumpVaneMode_Auto; // Default
            break;
    }

    outParams.mTemperature = static_cast<int>(targetTemp);
}

void IoHubClimate::IoHubToHAMqtt(const IoHubHeatpumpSettings &inParams,
                        HAClimateFanMode &outFanMode, 
                        HAClimateMode &outMode,
                        HAClimateVaneMode &outVaneMode,
                        float &outTargetTemp)
{
    if (inParams.mAction == HeatpumpAction_OFF) {
        outMode = HAClimateMode_Off;
    }

    if (inParams.mAction == HeatpumpAction_ON) {
        if (inParams.mMode == HeatpumpMode_Heat) {
            outMode = HAClimateMode_Heat;
        } else if (inParams.mMode == HeatpumpMode_Cold) {
            outMode = HAClimateMode_Cool;
        } else if (inParams.mMode == HeatpumpMode_Dry) {
            outMode = HAClimateMode_Dry;
        } else if (inParams.mMode == HeatpumpMode_Fan) {
            outMode = HAClimateMode_Fan_Only;
        } else {
            outMode = HAClimateMode_Auto;
        }
    }

    // Map IoHubHeatpumpFanSpeed to HAClimateFanMode
    if (inParams.mFanSpeed == HeatpumpFanSpeed_Auto) {
        outFanMode = HAClimateFanMode_Auto;
    } else if (inParams.mFanSpeed == HeatpumpFanSpeed_Low) {
        outFanMode = HAClimateFanMode_Low;
    } else if (inParams.mFanSpeed == HeatpumpFanSpeed_Med) {
        outFanMode = HAClimateFanMode_Medium;
    } else if (inParams.mFanSpeed == HeatpumpFanSpeed_High) {
        outFanMode = HAClimateFanMode_High;
    } else {
        outFanMode = HAClimateFanMode_Auto; // Default
    }

    // Map IoHubHeatpumpVaneMode to HAClimateVaneMode
    if (inParams.mVaneMode == HeatpumpVaneMode_Auto) {
        outVaneMode = HAClimateVaneMode_Auto;
    } else if (inParams.mVaneMode == HeatpumpVaneMode_1) {
        outVaneMode = HAClimateVaneMode_1;
    } else if (inParams.mVaneMode == HeatpumpVaneMode_2) {
        outVaneMode = HAClimateVaneMode_2;
    } else if (inParams.mVaneMode == HeatpumpVaneMode_3) {
        outVaneMode = HAClimateVaneMode_3;
    } else if (inParams.mVaneMode == HeatpumpVaneMode_4) {
        outVaneMode = HAClimateVaneMode_4;
    } else if (inParams.mVaneMode == HeatpumpVaneMode_5) {
        outVaneMode = HAClimateVaneMode_5;
    } else if (inParams.mVaneMode == HeatpumpVaneMode_Swing) {
        outVaneMode = HAClimateVaneMode_Swing;
    } else {
        outVaneMode = HAClimateVaneMode_Auto; // Default
    }

    outTargetTemp = static_cast<float>(inParams.mTemperature);
}