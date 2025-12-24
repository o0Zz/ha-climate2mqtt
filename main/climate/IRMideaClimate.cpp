#include "climate/IRMideaClimate.h"
#include "esp_log.h"

#define TAG "MideaClimate"

IRMideaClimate::IRMideaClimate(u8 txPin, u8 rxPin) :
    txPin(txPin),
    rxPin(rxPin)
{
    ESP_LOGI(TAG, "IRMideaClimate::IRMideaClimate ...");
}

IRMideaClimate::~IRMideaClimate() 
{
    iohub_heatpump_midea_uninit(&heatpumpCtx);
}

bool IRMideaClimate::setup()  
{
    ESP_LOGI(TAG, "Setting up Uart ...");

    iohub_digital_async_receiver_init(&receiverCtx, rxPin);

    ESP_LOGI(TAG, "Setting up Midea Heatpump ...");
	ret_code_t ret = iohub_heatpump_midea_init(&heatpumpCtx, &receiverCtx, txPin);
	if (ret != SUCCESS) {
		ESP_LOGE(TAG, "Failed to initialize Heatpump Midea (ret=%d)", ret);
		return false;
	}

    iohub_digital_async_receiver_start(&receiverCtx);
    return true;
}

bool IRMideaClimate::refresh()
{
    u16 receiverId;
    if ( !iohub_digital_async_receiver_has_packet_available(&receiverCtx, &receiverId) )
        return false;

    IoHubHeatpumpSettings settings;
    ESP_LOGI(TAG, "Refreshing Midea Heatpump state");

    if (receiverId != RECEIVER_HEATPUMP_MIDEA_ID)
    {
        ESP_LOGW(TAG, "Received packet for unknown receiver ID: 0x%04X", receiverId);
        return false;
    }

    bool ret = false;
    if (iohub_heatpump_midea_get_state(&heatpumpCtx, &settings) == SUCCESS) 
    {
        ret = IoHubToHAMqtt(settings, this->fan_mode, this->mode, this->vane_mode, this->target_temp);

        /*float room_temp;
        if (iohub_heatpump_midea_get_room_temperature(&heatpumpCtx, &room_temp) == SUCCESS) {
            this->room_temp = room_temp;
        }*/

    } else {
        ESP_LOGE(TAG, "Failed to get state from Heatpump Midea");
    }

    iohub_digital_async_receiver_packet_handled(&receiverCtx, receiverId);
    
    return ret;
}

bool IRMideaClimate::updateState() 
{
    IoHubHeatpumpSettings settings;

    HAMqttToIoHub(this->fan_mode, this->mode, this->vane_mode, this->target_temp, settings);

    ESP_LOGI(TAG, "Updating Midea Heatpump state: Action=%s, Mode=%d, Temp=%d, Fan=%d, Vane=%d",
             settings.mAction ? "ON": "OFF",
             settings.mMode,
             settings.mTemperature,
             settings.mFanSpeed,
             settings.mVaneMode);

    iohub_digital_async_receiver_stop(&receiverCtx);

    bool ret = iohub_heatpump_midea_set_state(&heatpumpCtx, &settings) == SUCCESS;

    iohub_digital_async_receiver_start(&receiverCtx);

    return ret;
}
