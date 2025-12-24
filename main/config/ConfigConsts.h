#pragma once

#include <map>
#include <string>

// All configuration key must be max 15 characters long

/* WiFi */
#define CONF_WIFI_SSID "wifi_ssid"
#define CONF_WIFI_PASSWORD "wifi_password"
#define CONF_WIFI_SECURITY "wifi_security"
#define CONF_WIFI_HOSTNAME "wifi_hostname"

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
    nullptr
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
    IR_MIDEA = 3
};

static const char *ClimateTypeStr[] = {
    "Unknown",
    "Mitsubishi",
    "Toshiba",
    "IR_Midea",
    nullptr
};

#define CONF_MQTT_UNIQUE_ID     "mqtt_unique_id"
#define CONF_MQTT_BROKER_URI    "mqtt_broker_uri"
#define CONF_MQTT_USER          "mqtt_user"
#define CONF_MQTT_PASSWORD      "mqtt_password"

static const std::map<std::string, const char*> ConfigList = {
    {CONF_WIFI_SSID, nullptr},
    {CONF_WIFI_PASSWORD, nullptr},
    {CONF_WIFI_SECURITY, "3"}, // Default to WPA2_PSK
    {CONF_WIFI_HOSTNAME, "climate2mqtt"},
    {CONF_CLIMATE_TYPE, "0"}, // Default to UNKNOWN
    {CONF_CLIMATE_POLLING_MS, "5000"},
    {CONF_CLIMATE_TX_PIN, "0"},
    {CONF_CLIMATE_RX_PIN, "1"},
    {CONF_MQTT_UNIQUE_ID, "climate_unit_1"},
    {CONF_MQTT_BROKER_URI, "mqtt://"},
    {CONF_MQTT_USER, nullptr},
    {CONF_MQTT_PASSWORD, nullptr}
};

static const std::map<std::string, const char* const*> ConfigValueList = {
    {CONF_CLIMATE_TYPE, ClimateTypeStr},
    {CONF_WIFI_SECURITY, WiFiSecurityTypeStr}
};