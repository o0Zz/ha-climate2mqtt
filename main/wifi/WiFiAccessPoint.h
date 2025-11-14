#pragma once

#include "esp_event_base.h"
#include "esp_wifi.h"

class WiFiAccessPoint
{
public:
    friend void event_handler_callback(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

public:
    WiFiAccessPoint();
    ~WiFiAccessPoint();

    bool setup(const std::string &ssid, const std::string &password);

    bool create();
    bool destroy();

    int clientCountConnected() const;

private:
    wifi_config_t wifi_config;
    int connected_clients = 0;

    void event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data);
};