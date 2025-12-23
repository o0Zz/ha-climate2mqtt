#pragma once

#include "esp_event_base.h"
#include "esp_wifi.h"

#include <string>
#include <cstdint>

class WiFiClient 
{
public:
    WiFiClient();
    ~WiFiClient();

    bool setup(const std::string &ssid, const std::string &password, const std::string &hostname = "", int max_retries = 5);

    bool connect();
    void disconnect();

    bool isConnected() const;

private:
    EventGroupHandle_t wifi_event_connect_group;

    int retry_num = 0;
    int max_retries = 0;
    bool auto_reconnect = true;
    bool connected = false;
    wifi_config_t wifi_config;

    static void event_handler_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    void event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data);
};