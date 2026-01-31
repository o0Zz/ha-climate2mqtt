#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <functional>
#include <memory>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "webserver/WebServer.h"

#include "mqtt/HAMqttClimateImpl.h"

#include "climate/UartClimate.h"

#include "climate/EmptyClimate.h"

#include "config/EspNVSConfig.h"
#include "config/ConfigConsts.h"

#include "wifi/WiFiClient.h"
#include "wifi/WiFiAccessPoint.h"

#include "systeminfo/ESP32SystemInfo.h"
#include "led/Led.h"
#include "version.h"
#include "logs/LogBuffer.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

#define AP_TIMEOUT_MS 60000
#define STATUS_LED_GPIO 8  // ESP32-C6 onboard RGB LED

struct ClimateFactoryEntry
{
	ClimateType type;
	const char *label;
	std::function<std::shared_ptr<IClimate>(climate_uart::transport::UartTransportESP32&)> factory;
};

// Map climate types to their corresponding UART protocol builders.
static const ClimateFactoryEntry CLIMATE_FACTORIES[] = {
	{ ClimateType::MITSUBISHI,    "Mitsubishi",    [](auto &transport) { return std::make_shared<UartClimate>(std::make_shared<climate_uart::protocols::Mitsubishi>(transport)); } },
	{ ClimateType::TOSHIBA,       "Toshiba",       [](auto &transport) { return std::make_shared<UartClimate>(std::make_shared<climate_uart::protocols::Toshiba>(transport)); } },
	{ ClimateType::DAIKIN_S21,    "Daikin",        [](auto &transport) { return std::make_shared<UartClimate>(std::make_shared<climate_uart::protocols::DaikinS21>(transport)); } },
	{ ClimateType::HITACHI_HLINK, "Hitachi HLink", [](auto &transport) { return std::make_shared<UartClimate>(std::make_shared<climate_uart::protocols::HitachiHLink>(transport)); } },
	{ ClimateType::LG_AIRCON,     "LG AirCon",     [](auto &transport) { return std::make_shared<UartClimate>(std::make_shared<climate_uart::protocols::LgAircon>(transport)); } },
	{ ClimateType::SHARP,         "Sharp",         [](auto &transport) { return std::make_shared<UartClimate>(std::make_shared<climate_uart::protocols::Sharp>(transport)); } }
};

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
            led.setColor(0, 0, 255); //Blue
            led.on();
            break;
        case LedStatus::OK:
            led.setColor(0, 255, 0); //Green
            led.on();
            break;
        case LedStatus::ERROR:
            led.setColor(255, 0, 0); //Red
            led.on();
            break;
        case LedStatus::WAITING_SETUP:
            led.setColor(0, 0, 255); //Green blinkings
            led.on(BLINK_PERIOD_US);
            break;
    }
}

extern "C" void app_main(void)
{
	LogBuffer::init();
	ESP_LOGI(TAG, "Starting Climate2MQTT application %s...", APP_VERSION);

	led::Led statusLed(STATUS_LED_GPIO);
	statusLed.setBrightness(10);

	status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - starting up

	auto config = std::make_unique<EspNVSConfig>();
	if (!config->load()) {
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
	std::unique_ptr<WebServer> webServer = std::make_unique<WebServer>(*config, std::move(systemInfo));
	webServer->start(80);  // Start on port 80

	auto wifiClient = std::make_unique<WiFiClient>();

	status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - connecting to WiFi
	if (!wifiClient->setup(config->getString(CONF_WIFI_SSID), config->getString(CONF_WIFI_PASSWORD), config->getString(CONF_WIFI_HOSTNAME))) {
		ESP_LOGE(TAG, "Failed to setup WiFi client");
		status_led_set(statusLed, LedStatus::ERROR);  // Red - WiFi setup error
		return;
	}

	if (!wifiClient->connect()) {
		ESP_LOGE(TAG, "Failed to connect to WiFi, creating access point for 1 min then reboot...");
		status_led_set(statusLed, LedStatus::ERROR);  // Red - WiFi connection failed
		
		WiFiAccessPoint wifiAP;
		if (!wifiAP.setup("Climate2MQTT_Setup", "", config->getString(CONF_WIFI_HOSTNAME))) {
			ESP_LOGE(TAG, "Failed to setup WiFi access point");
			return;
		}

		if (!wifiAP.create()) {
			ESP_LOGE(TAG, "Failed to create WiFi access point");
			return;
		}

		status_led_set(statusLed, LedStatus::WAITING_SETUP);  // Blinking red - awaiting user configuration

		
		uint32_t timeout_start = climate_uart::time_now_ms();
		while ((climate_uart::time_now_ms() - timeout_start) < AP_TIMEOUT_MS)
		{
			ESP_LOGI(TAG, "Access Point active for configuration (%d seconds remaining)...", (AP_TIMEOUT_MS - (climate_uart::time_now_ms() - timeout_start)) / 1000);
			vTaskDelay(pdMS_TO_TICKS(5000));

			if (wifiAP.clientCountConnected() > 0)
				timeout_start = climate_uart::time_now_ms(); // Reset timer if a client is connected
		}

		ESP_LOGI(TAG, "Rebooting to apply configuration...");
		esp_restart();
	}

	auto uartTransport = std::make_unique<climate_uart::transport::UartTransportESP32>(UART_NUM_1, config->getInt32(CONF_CLIMATE_TX_PIN), config->getInt32(CONF_CLIMATE_RX_PIN));
	const auto climateType = static_cast<ClimateType>(config->getInt32("climate_type", static_cast<int32_t>(ClimateType::UNKNOWN)));
	std::shared_ptr<IClimate> climate = nullptr;

	const auto factoryIt = std::find_if(std::begin(CLIMATE_FACTORIES), std::end(CLIMATE_FACTORIES), [climateType](const ClimateFactoryEntry &entry) {
		return entry.type == climateType;
	});

	if (factoryIt != std::end(CLIMATE_FACTORIES))
	{
		ESP_LOGI(TAG, "Using %s Climate interface", factoryIt->label);
		climate = factoryIt->factory(*uartTransport);
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
	}

	std::string unique_id = config->getString(CONF_MQTT_UNIQUE_ID);
	ESP_LOGI(TAG, "Initializing HA Climate MQTT (%s) ...", unique_id.c_str());

	HAMqttClimateImpl *climateMqtt = new HAMqttClimateImpl(climate, config->getString(CONF_MQTT_BROKER_URI), config->getString(CONF_MQTT_USER), config->getString(CONF_MQTT_PASSWORD), std::string("climate2mqtt/") + unique_id, unique_id);
    climateMqtt->start();

	status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - connecting to MQTT

	while (true) {
		//ESP_LOGD(TAG, "Refreshing state...");
		climateMqtt->refresh();

		// Update LED status based on connection states
		if (!wifiClient->isConnected()) {
			status_led_set(statusLed, LedStatus::ERROR);  // Red - WiFi disconnected
		} else if (!climateMqtt->isConnected()) {
			status_led_set(statusLed, LedStatus::CONNECTING);  // Blue - MQTT connecting/reconnecting
		} else {
			status_led_set(statusLed, LedStatus::OK);  // Green - all systems operational
		}
		vTaskDelay(pdMS_TO_TICKS(config->getInt32(CONF_CLIMATE_POLLING_MS, 1000)));
	}

	// Never reached
	climateMqtt->stop();
	delete climateMqtt;
}
