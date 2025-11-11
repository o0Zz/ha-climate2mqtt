#include "climate/ToshibaClimate.h"
#include "esp_log.h"

#define TAG "ToshibaClimate"

ToshibaClimate::ToshibaClimate(u8 txPin, u8 rxPin) :
    txPin(txPin),
    rxPin(rxPin)
{
    ESP_LOGI(TAG, "ToshibaClimate::ToshibaClimate ...");
}

ToshibaClimate::~ToshibaClimate() 
{
    iohub_heatpump_toshiba_uninit(&heatpumpCtx);
}

bool ToshibaClimate::setup()  
{
    ESP_LOGI(TAG, "Setting up Uart ...");

    ret_code_t ret = iohub_uart_init(&uartCtx, txPin, rxPin);
	if (ret != SUCCESS) {
		ESP_LOGE(TAG, "Failed to initialize UART for Heatpump Toshiba (ret=%d)", ret);
		return false;
	}

    ESP_LOGI(TAG, "Setting up Toshiba Heatpump ...");
	ret = iohub_heatpump_toshiba_init(&heatpumpCtx, &uartCtx);
	if (ret != SUCCESS) {
		ESP_LOGE(TAG, "Failed to initialize Heatpump Toshiba (ret=%d)", ret);
		return false;
	}

    return true;
}

bool ToshibaClimate::refresh() 
{
    ESP_LOGI(TAG, "Refreshing Toshiba Heatpump state");

	IoHubHeatpumpSettings settings;

	if (iohub_heatpump_toshiba_get_state(&heatpumpCtx, &settings) != SUCCESS) {
        ESP_LOGE(TAG, "Failed to get state from Heatpump Toshiba");
        return false;
    }

    bool ret = IoHubToHAMqtt(settings, this->fan_mode, this->mode, this->vane_mode, this->target_temp);

    /*float room_temp;
	if (iohub_heatpump_toshiba_get_room_temperature(&heatpumpCtx, &room_temp) == SUCCESS) {
        this->room_temp = room_temp;
    }*/

    return ret;
}

bool ToshibaClimate::updateState() 
{
    IoHubHeatpumpSettings settings;

    HAMqttToIoHub(this->fan_mode, this->mode, this->vane_mode, this->target_temp, settings);

    ESP_LOGI(TAG, "Updating Toshiba Heatpump state: Action=%s, Mode=%d, Temp=%d, Fan=%d, Vane=%d",
             settings.mAction ? "ON": "OFF",
             settings.mMode,
             settings.mTemperature,
             settings.mFanSpeed,
             settings.mVaneMode);

    return iohub_heatpump_toshiba_set_state(&heatpumpCtx, &settings) == SUCCESS;
}
