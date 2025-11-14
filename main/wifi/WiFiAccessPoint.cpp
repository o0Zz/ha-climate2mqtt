
#include "WiFiAccessPoint.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"

static void event_handler_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
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

bool WiFiAccessPoint::setup(const std::string &ssid, const std::string &password)
{
	esp_err_t err = esp_netif_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_netif_init failed: %s", esp_err_to_name(err));
        return false;
    }

	err = esp_event_loop_create_default();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_loop_create_default failed: %s", esp_err_to_name(err));
        return false;
    }

	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(err));
        return false;
    }

	esp_event_handler_instance_t instance_any_id;
	err = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler_callback, NULL, &instance_any_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_instance_register failed: %s", esp_err_to_name(err));
        return false;
    }
	
    esp_event_handler_instance_t instance_got_ip;
    err = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler_callback, NULL, &instance_got_ip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_instance_register failed: %s", esp_err_to_name(err));
        return false;
    }

    memset(&wifi_config, 0, sizeof(wifi_config_t));
	memcpy(wifi_config.ap.ssid, ssid.c_str(), ssid.length());
	memcpy(wifi_config.ap.password, password.c_str(), password.length());
	wifi_config.ap.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.pmf_cfg.capable = true;
	wifi_config.ap.pmf_cfg.required = false;

	esp_err_t err = esp_wifi_set_mode(WIFI_MODE_AP);
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
	err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return false;
    }
}

void WiFiAccessPoint::destroy()
{
    esp_wifi_stop();
    connected_clients = 0;
}