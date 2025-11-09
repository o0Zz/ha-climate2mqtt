#include "climate/MitsubishiClimate.h"
#include "esp_log.h"

#define TAG "MitsubishiClimate"

MitsubishiClimate::MitsubishiClimate(u8 txPin, u8 rxPin) :
    txPin(txPin),
    rxPin(rxPin)
{
}

MitsubishiClimate::~MitsubishiClimate() 
{
    iohub_heatpump_mitsubishi_uninit(&heatpumpCtx);
}

bool MitsubishiClimate::setup()  
{
    ret_code_t ret = iohub_uart_init(&uartCtx, txPin, rxPin);
	if (ret != SUCCESS) {
		ESP_LOGE(TAG, "Failed to initialize UART for Heatpump Mitsubishi (ret=%d)", ret);
		return false;
	}

	ret = iohub_heatpump_mitsubishi_init(&heatpumpCtx, &uartCtx);
	if (ret != SUCCESS) {
		ESP_LOGE(TAG, "Failed to initialize Heatpump Mitsubishi (ret=%d)", ret);
		return false;
	}

    return true;
}

bool MitsubishiClimate::refresh() 
{
    ESP_LOGI(TAG, "Refreshing Mitsubishi Heatpump state");

	IoHubHeatpumpSettings settings;

	if (iohub_heatpump_mitsubishi_get_state(&heatpumpCtx, &settings) != SUCCESS) {
        ESP_LOGE(TAG, "Failed to get state from Heatpump Mitsubishi");
        return false;
    }

    IoHubToHAMqtt(settings, this->fan_mode, this->mode, this->vane_mode, this->target_temp);

    float room_temp;
	if (iohub_heatpump_mitsubishi_get_room_temperature(&heatpumpCtx, &room_temp) == SUCCESS) {
        this->room_temp = room_temp;
    }

    return true;
}

bool MitsubishiClimate::updateState() 
{
    IoHubHeatpumpSettings settings;

    HAMqttToIoHub(this->fan_mode, this->mode, this->vane_mode, this->target_temp, settings);

    ESP_LOGI(TAG, "Updating Mitsubishi Heatpump state: Action=%s, Mode=%d, Temp=%d, Fan=%d, Vane=%d",
             settings.mAction ? "ON": "OFF",
             settings.mMode,
             settings.mTemperature,
             settings.mFanSpeed,
             settings.mVaneMode);

    return iohub_heatpump_mitsubishi_set_state(&heatpumpCtx, &settings) == SUCCESS;
}
