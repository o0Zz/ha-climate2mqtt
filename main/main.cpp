#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "iohub.h"

#include "webserver/WebServer.h"

#include "mqtt/HAMqttClimateImpl.h"

#include "climate/MitsubishiClimate.h"
#include "climate/ToshibaClimate.h"
#include "climate/IRMideaClimate.h"
#include "climate/EmptyClimate.h"

#include "config/EspNVSConfig.h"
#include "config/ConfigConsts.h"

#include "wifi/WiFiClient.h"
#include "wifi/WiFiAccessPoint.h"

#include "systeminfo/ESP32SystemInfo.h"
#include "led/Led.h"
#include "version.h"

static const char *TAG = "MAIN";

#define AP_TIMEOUT_MS 60000
#define STATUS_LED_GPIO 8  // ESP32-C6 onboard RGB LED

enum class LedStatus {
    OFF,            // LED off
    CONNECTING,     // Blue - connecting to WiFi/MQTT/Climate
    OK,             // Green - everything working
    ERROR,          // Red - connection error
    WAITING_SETUP   // Blinking red - waiting for user setup
};

void status_led_set(led::Led& led, LedStatus status)
{
    constexpr uint64_t BLINK_PERIOD_US = 500000;

    switch (status) {
        case LedStatus::OFF:
            led.off();
            break;
        case LedStatus::CONNECTING:
            led.setColor(0, 0, 255);
            led.on();
            break;
        case LedStatus::OK:
            led.setColor(0, 255, 0);
            led.on();
            break;
        case LedStatus::ERROR:
            led.setColor(255, 0, 0);
            led.on();
            break;
        case LedStatus::WAITING_SETUP:
            led.setColor(0, 0, 255);
            led.on(BLINK_PERIOD_US);
            break;
    }
}

extern "C" void app_main(void)
{
	ESP_LOGI(TAG, "Starting Climate2MQTT application %s...", APP_VERSION);
	iohub_platform_init();

	led::Led statusLed(STATUS_LED_GPIO);
	statusLed.setBrightness(10);

	status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - starting up

	EspNVSConfig config;
	if (!config.load()) {
		ESP_LOGE(TAG, "Failed to load configuration from NVS");
		status_led_set(statusLed, LedStatus::ERROR);  // Red - config error
		return;
	}

    esp_err_t err = esp_netif_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_netif_init failed: %s", esp_err_to_name(err));
    }

	err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "esp_event_loop_create_default failed: %s", esp_err_to_name(err));
    }	

    std::unique_ptr<systeminfo::ISystemInfo> systemInfo = std::make_unique<systeminfo::ESP32SystemInfo>();
	WebServer webServer(config, std::move(systemInfo));
	webServer.start(80);  // Start on port 80

	WiFiClient wifiClient;

	status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - connecting to WiFi
	if (!wifiClient.setup(config.getString(CONF_WIFI_SSID), config.getString(CONF_WIFI_PASSWORD), config.getString(CONF_WIFI_HOSTNAME))) {
		ESP_LOGE(TAG, "Failed to setup WiFi client");
		status_led_set(statusLed, LedStatus::ERROR);  // Red - WiFi setup error
		return;
	}

	if (!wifiClient.connect()) {
		ESP_LOGE(TAG, "Failed to connect to WiFi, creating access point for 1 min then reboot...");
		status_led_set(statusLed, LedStatus::ERROR);  // Red - WiFi connection failed
		
		WiFiAccessPoint wifiAP;
		if (!wifiAP.setup("Climate2MQTT_Setup", "", config.getString(CONF_WIFI_HOSTNAME))) {
			ESP_LOGE(TAG, "Failed to setup WiFi access point");
			return;
		}

		if (!wifiAP.create()) {
			ESP_LOGE(TAG, "Failed to create WiFi access point");
			return;
		}

		status_led_set(statusLed, LedStatus::WAITING_SETUP);  // Blinking red - awaiting user configuration

		u32 timeout_start = IOHUB_TIMER_START();
		while (IOHUB_TIMER_ELAPSED(timeout_start) < AP_TIMEOUT_MS)
		{
			ESP_LOGI(TAG, "Access Point active for configuration (%d seconds remaining)...", (AP_TIMEOUT_MS - IOHUB_TIMER_ELAPSED(timeout_start)) / 1000);
			vTaskDelay(pdMS_TO_TICKS(5000));

			if (wifiAP.clientCountConnected() > 0)
				timeout_start = IOHUB_TIMER_START(); // Reset timer if a client is connected
		}

		ESP_LOGI(TAG, "Rebooting to apply configuration...");
		esp_restart();
	}

	
	std::shared_ptr<IClimate> climate = nullptr;
	if (config.getInt32("climate_type", (int32_t)ClimateType::UNKNOWN) == (int32_t)ClimateType::MITSUBISHI)
	{
		ESP_LOGI(TAG, "Using Mitsubishi Climate interface");
		climate = std::make_shared<MitsubishiClimate>(config.getInt32(CONF_CLIMATE_TX_PIN), config.getInt32(CONF_CLIMATE_RX_PIN));
	}
	else if (config.getInt32("climate_type", (int32_t)ClimateType::UNKNOWN) == (int32_t)ClimateType::TOSHIBA)
	{
		ESP_LOGI(TAG, "Using Toshiba Climate interface");
		climate = std::make_shared<ToshibaClimate>(config.getInt32(CONF_CLIMATE_TX_PIN), config.getInt32(CONF_CLIMATE_RX_PIN));
	}
	else if (config.getInt32("climate_type", (int32_t)ClimateType::UNKNOWN) == (int32_t)ClimateType::IR_MIDEA)
	{
		ESP_LOGI(TAG, "Using Midea Climate interface");
		climate = std::make_shared<IRMideaClimate>(config.getInt32(CONF_CLIMATE_TX_PIN), config.getInt32(CONF_CLIMATE_RX_PIN));
	}
	else
	{
		ESP_LOGE(TAG, "Invalid climate_type configuration !");
		climate = std::make_shared<EmptyClimate>();
	}

	if (!climate->setup())
	{
		ESP_LOGE(TAG, "Failed to setup climate ...");
		status_led_set(statusLed, LedStatus::ERROR);  // Red - climate setup error
		//TODO: return error to webinterface ?
	}

	std::string unique_id = config.getString(CONF_MQTT_UNIQUE_ID);
	ESP_LOGI(TAG, "Initializing HA Climate MQTT (%s) ...", unique_id.c_str());

	HAMqttClimateImpl *climateMqtt = new HAMqttClimateImpl(climate, config.getString(CONF_MQTT_BROKER_URI), config.getString(CONF_MQTT_USER), config.getString(CONF_MQTT_PASSWORD), std::string("climate2mqtt/") + unique_id, unique_id);
    climateMqtt->start();

	status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - connecting to MQTT

	while (true) {
		//ESP_LOGD(TAG, "Refreshing state...");
		climateMqtt->refresh();

		// Update LED status based on connection states
		if (!wifiClient.isConnected()) {
			status_led_set(statusLed, LedStatus::ERROR);  // Red - WiFi disconnected
		} else if (!climateMqtt->isConnected()) {
			status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - MQTT connecting/reconnecting
		} else {
			status_led_set(statusLed, LedStatus::OK);  // Green - all systems operational
		}

		vTaskDelay(pdMS_TO_TICKS(config.getInt32(CONF_CLIMATE_POLLING_MS, 1000)));
	}

	// Never reached
	climateMqtt->stop();
	delete climateMqtt;
}
