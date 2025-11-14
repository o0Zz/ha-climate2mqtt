#pragma once

#include "esp_event_base.h"
#include "esp_wifi.h"

class WiFiClient 
{
public:
    friend void event_handler_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

public:
    WiFiClient();
    ~WiFiClient();

    bool setup(const std::string &ssid, const std::string &password, int max_retries = 5);

    bool connect();
    bool disconnect();
    bool isConnected() const;

private:
    EventGroupHandle_t wifi_event_connect_group;

    int retry_num = 0;
    int max_retries = 0;
    bool auto_reconnect = true;
    wifi_config_t wifi_config;

    void event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data);
};