#pragma once

#include "esp_event_base.h"
#include "esp_wifi.h"

#include <string>
#include <cstdint>

class WiFiAccessPoint
{
public:
    WiFiAccessPoint();
    ~WiFiAccessPoint();

    bool setup(const std::string &ssid, const std::string &password, const std::string &hostname = "");

    bool create();
    void destroy();

    int8_t clientCountConnected() const;

private:
    wifi_config_t wifi_config;
    int8_t connected_clients = 0;

    static void event_handler_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    void event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data);
};