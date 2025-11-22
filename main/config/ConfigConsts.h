#pragma once

#include <map>

// All configuration key must be max 15 characters long

/* WiFi */
#define CONF_WIFI_SSID "wifi_ssid"
#define CONF_WIFI_PASSWORD "wifi_password"
#define CONF_WIFI_SECURITY "wifi_security"

enum class WiFiSecurityType {
    OPEN = 0,
    WEP = 1,
    WPA_PSK = 2,
    WPA2_PSK = 3,
    WPA_WPA2_PSK = 4,
    WPA3_PSK = 5
};

static const char* WiFiSecurityTypeStr[] = {
    "Open",
    "WEP",
    "WPA-PSK",
    "WPA2-PSK",
    "WPA/WPA2-PSK",
    "WPA3-PSK",
    NULL
};

/* Climate */
#define CONF_CLIMATE_TYPE "climate_type"
#define CONF_CLIMATE_POLLING_MS "climate_poll_ms"
#define CONF_CLIMATE_TX_PIN "climate_tx_pin"
#define CONF_CLIMATE_RX_PIN "climate_rx_pin"

enum class ClimateType {
    UNKNOWN = 0,
    MITSUBISHI = 1,
    TOSHIBA = 2,
    MIDEA = 3
};

static const char *ClimateTypeStr[] = {
    "Unknown",
    "Mitsubishi",
    "Toshiba",
    "Midea",
    NULL
};

#define CONF_MQTT_UNIQUE_ID     "mqtt_unique_id"
#define CONF_MQTT_BROKER_URI    "mqtt_broker_uri"
#define CONF_MQTT_USER          "mqtt_user"
#define CONF_MQTT_PASSWORD      "mqtt_password"

static const char* ConfigList[] = {
    CONF_WIFI_SSID,
    CONF_WIFI_PASSWORD,
    CONF_WIFI_SECURITY,
    CONF_CLIMATE_TYPE,
    CONF_CLIMATE_POLLING_MS,
    CONF_CLIMATE_TX_PIN,
    CONF_CLIMATE_RX_PIN,
    CONF_MQTT_UNIQUE_ID,
    CONF_MQTT_BROKER_URI,
    CONF_MQTT_USER,
    CONF_MQTT_PASSWORD,
    NULL
};

static const std::map<std::string, const char* const*> ConfigValueList = {
    {"climate_type_list", ClimateTypeStr},
    {"wifi_security_list", WiFiSecurityTypeStr}
};