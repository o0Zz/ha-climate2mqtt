#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "iohub.h"

#include "mqtt/HAMqttClimateImpl.h"
#include "climate/MitsubishiClimate.h"
#include "climate/ToshibaClimate.h"
#include "climate/MideaClimate.h"
#include "climate/EmptyClimate.h"

#include "config/EspNVSConfig.h"
#include "config/ConfigConsts.h"

#include "wifi/WiFiClient.h"
#include "wifi/WiFiAccessPoint.h"

static const char *TAG = "MAIN";

extern "C" void app_main(void)
{
	iohub_platform_init();

	IConfig *config = new EspNVSConfig();
	if (!config->load()) {
		ESP_LOGE(TAG, "Failed to load configuration from NVS");
		return;
	}

	WiFiClient wifiClient;

	if (!wifiClient.setup(config->getString(CONF_WIFI_SSID), config->getString(CONF_WIFI_PASSWORD))) {
		ESP_LOGE(TAG, "Failed to setup WiFi client");
		return;
	}

	if (!wifiClient.connect()) {
		ESP_LOGE(TAG, "Failed to connect to WiFi, creating access point for 1 min then reboot...");
		
		WiFiAccessPoint wifiAP;
		if (!wifiAP.setup("Climate2MQTT_Setup", "")) {
			ESP_LOGE(TAG, "Failed to setup WiFi access point");
			return;
		}

		if (!wifiAP.create()) {
			ESP_LOGE(TAG, "Failed to create WiFi access point");
			return;
		}

		for (int i = 0; i < 60; i++) {
			ESP_LOGI(TAG, "Access Point active for configuration (%d seconds remaining)...", 60 - i);
			vTaskDelay(pdMS_TO_TICKS(1000));

			if (wifiAP.clientCountConnected() > 0)
				i = 0; // Reset timer if a client is connected
		}

		ESP_LOGI(TAG, "Rebooting to apply configuration...");
		esp_restart();
	}

	
	std::shared_ptr<IClimate> climate = nullptr;
	if (config->getInt32("climate_type", 1) == 1)
	{
		ESP_LOGI(TAG, "Using Mitsubishi Climate interface");
		climate = std::make_shared<MitsubishiClimate>(config->getInt32(CONF_CLIMATE_UART_TX_PIN, 0), config->getInt32(CONF_CLIMATE_UART_RX_PIN, 1));
	}
	else if (config->getInt32("climate_type", 1) == 2)
	{
		ESP_LOGI(TAG, "Using Toshiba Climate interface");
		climate = std::make_shared<ToshibaClimate>(config->getInt32(CONF_CLIMATE_UART_TX_PIN, 0), config->getInt32(CONF_CLIMATE_UART_RX_PIN, 1));
	}
	else if (config->getInt32("climate_type", 1) == 3)
	{
		ESP_LOGI(TAG, "Using Midea Climate interface");
		climate = std::make_shared<MideaClimate>(config->getInt32(CONF_CLIMATE_UART_TX_PIN, 0), config->getInt32(CONF_CLIMATE_UART_RX_PIN, 1));
	}
	else
	{
		ESP_LOGE(TAG, "Invalid climate_type configuration !");
		climate = std::make_shared<EmptyClimate>(0, 1);
	}

	if (!climate->setup())
	{
		ESP_LOGE(TAG, "Failed to setup Mitsubishi Climate interface !");
		return;
	}

	std::string unique_id = config->getString(CONF_MQTT_UNIQUE_ID, "ac_unit_1");
	ESP_LOGI(TAG, "Initializing HA Climate MQTT (%s) ...", unique_id.c_str());

	HAMqttClimateImpl *climateMqtt = new HAMqttClimateImpl(climate, config->getString(CONF_MQTT_BROKER_URI), config->getString(CONF_MQTT_USER), config->getString(CONF_MQTT_PASSWORD), std::string("climate2mqtt/") + unique_id, unique_id);
    climateMqtt->start();

	while (true) {
		vTaskDelay(pdMS_TO_TICKS(config->getInt32(CONF_CLIMATE_POLLING_MS, 1000)));

		//ESP_LOGD(TAG, "Refreshing state...");
		climateMqtt->refresh();
	}

	// Never reached
	climateMqtt->stop();
	delete climateMqtt;
}
