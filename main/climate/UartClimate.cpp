#include "climate/UartClimate.h"
#include "esp_log.h"

#define TAG "UartClimate"

using climate_uart::ClimateSettings;
using climate_uart::HeatpumpAction;
using climate_uart::HeatpumpFanSpeed;
using climate_uart::HeatpumpMode;
using climate_uart::HeatpumpVaneMode;

UartClimate::UartClimate(std::shared_ptr<climate_uart::ClimateInterface> interface) :
    interface(interface),
    room_temp(0.0f),
    target_temp(22.0f),
    mode(HAClimateMode_Off),
    fan_mode(HAClimateFanMode_Auto),
    action(HAClimateAction_Idle),
    vane_mode(HAClimateVaneMode_Auto),
    is_locked(false)
{

}

void UartClimate::lock()
{
    is_locked = true;
}

void UartClimate::unlock()
{
    is_locked = false;
    updateState();
}

float UartClimate::getTargetTemperature() const
{
    return target_temp;
}

bool UartClimate::setTargetTemperature(float temperature)
{
    target_temp = temperature;

    if (is_locked)
        return true;

    return updateState();
}

HAClimateMode UartClimate::getMode() const
{
    return mode;
}

bool UartClimate::setMode(HAClimateMode mode)
{
    this->mode = mode;

    if (is_locked)
        return true;

    return updateState();
}

HAClimateFanMode UartClimate::getFanMode() const
{
    return fan_mode;
}

bool UartClimate::setFanMode(HAClimateFanMode fanMode)
{
    this->fan_mode = fanMode;

    if (is_locked)
        return true;

    return updateState();
}

HAClimateVaneMode UartClimate::getVaneMode() const
{
    return vane_mode;
}

bool UartClimate::setVaneMode(HAClimateVaneMode vaneMode)
{
    this->vane_mode = vaneMode;

    if (is_locked)
        return true;

    return updateState();
}

bool UartClimate::updateState() 
{
    climate_uart::ClimateSettings settings;

    HAMqttToUart(this->fan_mode, this->mode, this->vane_mode, this->target_temp, settings);
    ESP_LOGI(TAG, "Updating Climate state: Action=%s, Mode=%d, Temp=%d, Fan=%d, Vane=%d",
             settings.action == climate_uart::HeatpumpAction::On ? "ON": "OFF",
             settings.mode,
             settings.temperature,
             settings.fanSpeed,
             settings.vaneMode);
   
    if (interface->setState(settings) != climate_uart::kSuccess) {
        ESP_LOGE(TAG, "Failed to set state on heatpump");
        return false;
    }

    return true;
}

bool UartClimate::HAMqttToUart(HAClimateFanMode fanMode,
                               HAClimateMode mode,
                               HAClimateVaneMode vaneMode,
                               float targetTemp,
                               ClimateSettings &outParams)
{
    switch (fanMode) {
        case HAClimateFanMode_Auto:
            outParams.fanSpeed = HeatpumpFanSpeed::Auto;
            break;
        case HAClimateFanMode_Quiet:
            outParams.fanSpeed = HeatpumpFanSpeed::Quiet;
            break;
        case HAClimateFanMode_Low:
            outParams.fanSpeed = HeatpumpFanSpeed::Low;
            break;
        case HAClimateFanMode_Medium:
            outParams.fanSpeed = HeatpumpFanSpeed::Med;
            break;
        case HAClimateFanMode_High:
            outParams.fanSpeed = HeatpumpFanSpeed::High;
            break;
        default:
            outParams.fanSpeed = HeatpumpFanSpeed::Auto;
            break;
    }

    if (mode == HAClimateMode_Off) {
        outParams.action = HeatpumpAction::Off;
        outParams.mode = HeatpumpMode::None;
    } else {
        outParams.action = HeatpumpAction::On;
        switch (mode) {
            case HAClimateMode_Heat:
                outParams.mode = HeatpumpMode::Heat;
                break;
            case HAClimateMode_Cool:
                outParams.mode = HeatpumpMode::Cold;
                break;
            case HAClimateMode_Dry:
                outParams.mode = HeatpumpMode::Dry;
                break;
            case HAClimateMode_Fan_Only:
                outParams.mode = HeatpumpMode::Fan;
                break;
            case HAClimateMode_Auto:
                outParams.mode = HeatpumpMode::Auto;
                break;
            default:
                outParams.mode = HeatpumpMode::Auto;
                break;
        }
    }

    switch (vaneMode) {
        case HAClimateVaneMode_Auto:
            outParams.vaneMode = HeatpumpVaneMode::Auto;
            break;
        case HAClimateVaneMode_1:
            outParams.vaneMode = HeatpumpVaneMode::V1;
            break;
        case HAClimateVaneMode_2:
            outParams.vaneMode = HeatpumpVaneMode::V2;
            break;
        case HAClimateVaneMode_3:
            outParams.vaneMode = HeatpumpVaneMode::V3;
            break;
        case HAClimateVaneMode_4:
            outParams.vaneMode = HeatpumpVaneMode::V4;
            break;
        case HAClimateVaneMode_5:
            outParams.vaneMode = HeatpumpVaneMode::V5;
            break;
        case HAClimateVaneMode_Swing:
            outParams.vaneMode = HeatpumpVaneMode::Swing;
            break;
        default:
            outParams.vaneMode = HeatpumpVaneMode::Auto;
            break;
    }

    outParams.temperature = static_cast<int>(targetTemp);
    return true;
}

bool UartClimate::UartToHAMqtt(const ClimateSettings &inParams,
                               HAClimateFanMode &outFanMode,
                               HAClimateMode &outMode,
                               HAClimateVaneMode &outVaneMode,
                               float &outTargetTemp)
{
    ESP_LOGI(TAG, "UartToHAMqtt IN Action=%d, Mode=%d, FanSpeed=%d, VaneMode=%d, Temp=%d",
                static_cast<int>(inParams.action),
                static_cast<int>(inParams.mode),
                static_cast<int>(inParams.fanSpeed),
                static_cast<int>(inParams.vaneMode),
                inParams.temperature);

    if (inParams.action == HeatpumpAction::Direction) {
        ESP_LOGD(TAG, "UartToHAMqtt Direction mode not supported");
        return false;
    }

    if (inParams.action == HeatpumpAction::Off) {
        outMode = HAClimateMode_Off;
    }

    if (inParams.action == HeatpumpAction::On) {
        if (inParams.mode == HeatpumpMode::Heat) {
            outMode = HAClimateMode_Heat;
        } else if (inParams.mode == HeatpumpMode::Cold) {
            outMode = HAClimateMode_Cool;
        } else if (inParams.mode == HeatpumpMode::Dry) {
            outMode = HAClimateMode_Dry;
        } else if (inParams.mode == HeatpumpMode::Fan) {
            outMode = HAClimateMode_Fan_Only;
        } else {
            outMode = HAClimateMode_Auto;
        }
    }

    if (inParams.fanSpeed == HeatpumpFanSpeed::Auto) {
        outFanMode = HAClimateFanMode_Auto;
    } else if (inParams.fanSpeed == HeatpumpFanSpeed::Low) {
        outFanMode = HAClimateFanMode_Low;
    } else if (inParams.fanSpeed == HeatpumpFanSpeed::Med) {
        outFanMode = HAClimateFanMode_Medium;
    } else if (inParams.fanSpeed == HeatpumpFanSpeed::High) {
        outFanMode = HAClimateFanMode_High;
    } else if (inParams.fanSpeed == HeatpumpFanSpeed::Quiet) {
        outFanMode = HAClimateFanMode_Quiet;
    } else {
        outFanMode = HAClimateFanMode_Auto;
    }

    if (inParams.vaneMode == HeatpumpVaneMode::Auto) {
        outVaneMode = HAClimateVaneMode_Auto;
    } else if (inParams.vaneMode == HeatpumpVaneMode::V1) {
        outVaneMode = HAClimateVaneMode_1;
    } else if (inParams.vaneMode == HeatpumpVaneMode::V2) {
        outVaneMode = HAClimateVaneMode_2;
    } else if (inParams.vaneMode == HeatpumpVaneMode::V3) {
        outVaneMode = HAClimateVaneMode_3;
    } else if (inParams.vaneMode == HeatpumpVaneMode::V4) {
        outVaneMode = HAClimateVaneMode_4;
    } else if (inParams.vaneMode == HeatpumpVaneMode::V5) {
        outVaneMode = HAClimateVaneMode_5;
    } else if (inParams.vaneMode == HeatpumpVaneMode::Swing) {
        outVaneMode = HAClimateVaneMode_Swing;
    } else {
        outVaneMode = HAClimateVaneMode_Auto;
    }

    outTargetTemp = static_cast<float>(inParams.temperature);

    ESP_LOGI(TAG, "UartToHAMqtt OUT Mode=%d, FanMode=%d, VaneMode=%d, Temp=%f",
                outMode,
                outFanMode,
                outVaneMode,
                outTargetTemp);

    return true;
}

bool UartClimate::setup()
{
    if (interface->init() != climate_uart::kSuccess) {
        ESP_LOGE(TAG, "Failed to initialize climate interface");
        return false;
    }
    return true;
}

bool UartClimate::refresh()
{
    climate_uart::ClimateSettings settings;
    if (interface->getState(settings) != climate_uart::kSuccess) {
        ESP_LOGE(TAG, "Failed to get state from climate interface");
        return false;
    }

    HAClimateFanMode fanMode;
    HAClimateMode mode;
    HAClimateVaneMode vaneMode;
    float targetTemp;
    if (!UartToHAMqtt(settings, fanMode, mode, vaneMode, targetTemp)) {
        ESP_LOGE(TAG, "Failed to convert UART settings to HA MQTT");
        return false;
    }

    this->fan_mode = fanMode;
    this->mode = mode;
    this->vane_mode = vaneMode;
    this->target_temp = targetTemp;

    if (interface->getRoomTemperature(this->room_temp) != climate_uart::kSuccess) {
        ESP_LOGW(TAG, "Failed to get room temperature");
        // Not critical, continue
    }

    return true;
}