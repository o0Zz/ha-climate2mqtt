
#include "WiFiAccessPoint.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_netif.h"

#include <string.h>

#define TAG "WiFiAccessPoint"

void WiFiAccessPoint::event_handler_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    WiFiAccessPoint* client = static_cast<WiFiAccessPoint*>(arg);
    client->event_handler(event_base, event_id, event_data);
}

void WiFiAccessPoint::event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) 
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
        connected_clients++;
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) 
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
        connected_clients--;
    }  
}

WiFiAccessPoint::WiFiAccessPoint()
{
}

WiFiAccessPoint::~WiFiAccessPoint()
{
}

bool WiFiAccessPoint::setup(const std::string &ssid, const std::string &password, const std::string &hostname)
{
    esp_netif_t *netif = esp_netif_create_default_wifi_ap();

    if (netif == nullptr) {
        ESP_LOGE(TAG, "Failed to create default WiFi AP interface");
        return false;
    }

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_err_t err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(err));
        return false;
    }

    if (!hostname.empty()) {
        err = esp_netif_set_hostname(netif, hostname.c_str());
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "esp_netif_set_hostname failed: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "WiFi AP hostname set to %s", hostname.c_str());
        }
    }

	esp_event_handler_instance_t instance_any_id;
	err = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiAccessPoint::event_handler_callback, this, &instance_any_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_instance_register failed: %s", esp_err_to_name(err));
        return false;
    }
	
    esp_event_handler_instance_t instance_got_ip;
    err = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFiAccessPoint::event_handler_callback, this, &instance_got_ip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_instance_register failed: %s", esp_err_to_name(err));
        return false;
    }

    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strlcpy(reinterpret_cast<char*>(wifi_config.ap.ssid), ssid.c_str(), sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = strnlen(reinterpret_cast<const char*>(wifi_config.ap.ssid), sizeof(wifi_config.ap.ssid));
    strlcpy(reinterpret_cast<char*>(wifi_config.ap.password), password.c_str(), sizeof(wifi_config.ap.password));
    wifi_config.ap.authmode = password.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = 1;
	wifi_config.ap.pmf_cfg.capable = true;
	wifi_config.ap.pmf_cfg.required = false;

	err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %s", esp_err_to_name(err));
        return false;
    }

	err = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config failed: %s", esp_err_to_name(err));
        return false;
    }

    return true;
}

bool WiFiAccessPoint::create()
{
    ESP_LOGI(TAG, "Creating Access Point SSID:%s", wifi_config.ap.ssid);

    connected_clients = 0;
	esp_err_t err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return false;
    }

    return true;
}

void WiFiAccessPoint::destroy()
{
    esp_wifi_stop();
    connected_clients = 0;
}

int8_t WiFiAccessPoint::clientCountConnected() const
{
    return connected_clients;
}