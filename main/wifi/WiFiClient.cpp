
#include "WiFiClient.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

#include <string.h>

#define TAG "WiFiClient"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

void WiFiClient::event_handler_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    WiFiClient* client = static_cast<WiFiClient*>(arg);
    client->event_handler(event_base, event_id, event_data);
}

void WiFiClient::event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        connected = false;
        
        if (wifi_event_connect_group != nullptr)
        {
            ESP_LOGE(TAG, "WiFi Connection attempt failed %d/%d", retry_num + 1, max_retries);
            if (++retry_num > max_retries)
            {
                xEventGroupSetBits(wifi_event_connect_group, WIFI_FAIL_BIT);
            }
            else
            {
                esp_wifi_connect();
            }
        }
        else if (auto_reconnect) 
        {
            ESP_LOGI(TAG, "WiFi disconnected ! Reconnecting to WiFi access point ...");
            esp_wifi_connect();
        }
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        connected = true;
        
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

        ESP_LOGI(TAG, "Got IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        if (wifi_event_connect_group != nullptr)
            xEventGroupSetBits(wifi_event_connect_group, WIFI_CONNECTED_BIT);
    }
}

WiFiClient::WiFiClient() :
    wifi_event_connect_group(nullptr),
    retry_num(0),
    max_retries(0),
    auto_reconnect(true)
{
}

WiFiClient::~WiFiClient()
{
}

bool WiFiClient::setup(const std::string &ssid, const std::string &password, const std::string &hostname, int max_retries)
{
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();

    if (netif == nullptr) {
        ESP_LOGE(TAG, "Failed to create default WiFi STA interface");
        return false;
    }

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_err_t err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(err));
        return false;
    }

	esp_event_handler_instance_t instance_any_id;
	err = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiClient::event_handler_callback, this, &instance_any_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_instance_register failed: %s", esp_err_to_name(err));
        return false;
    }
	
    esp_event_handler_instance_t instance_got_ip;
    err = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFiClient::event_handler_callback, this, &instance_got_ip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_instance_register failed: %s", esp_err_to_name(err));
        return false;
    }

    if (!hostname.empty()) {
        err = esp_netif_set_hostname(netif, hostname.c_str());
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "esp_netif_set_hostname failed: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "WiFi hostname set to %s", hostname.c_str());
        }
    }


    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strlcpy(reinterpret_cast<char*>(wifi_config.sta.ssid), ssid.c_str(), sizeof(wifi_config.sta.ssid));
    strlcpy(reinterpret_cast<char*>(wifi_config.sta.password), password.c_str(), sizeof(wifi_config.sta.password));
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.pmf_cfg.capable = true;
	wifi_config.sta.pmf_cfg.required = false;

	err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %s", esp_err_to_name(err));
        return false;
    }

	err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config failed: %s", esp_err_to_name(err));
        return false;
    }

    this->max_retries = max_retries;
    return true;
}


bool WiFiClient::connect()
{
    if (isConnected()) {
        ESP_LOGI(TAG, "Already connected to WiFi SSID:%s", wifi_config.sta.ssid);
        return true;
    }

    if (wifi_config.sta.ssid[0] == '\0') {
        ESP_LOGE(TAG, "WiFi SSID is empty. Cannot connect.");
        return false;
    }

    ESP_LOGI(TAG, "Connecting to SSID:%s", wifi_config.sta.ssid);

    wifi_event_connect_group = xEventGroupCreate();
    retry_num = 0;

	esp_err_t err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return false;
    }

	EventBits_t bits = xEventGroupWaitBits(wifi_event_connect_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "Connected to access point SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
	} else {
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
	}

    vEventGroupDelete(wifi_event_connect_group);
    wifi_event_connect_group = nullptr;

    return (bits & WIFI_CONNECTED_BIT) != 0;
}

void WiFiClient::disconnect()
{
    auto_reconnect = false;
    esp_wifi_disconnect();
    esp_wifi_stop();
}

bool WiFiClient::isConnected() const
{
    return connected;
}