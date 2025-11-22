#pragma once

#define CONF_WIFI_SSID "wifi_ssid"
#define CONF_WIFI_PASSWORD "wifi_password"
#define CONF_WIFI_SECURITY_TYPE "wifi_security_type"

#define CONF_CLIMATE_TYPE "climate_type"
#define CONF_CLIMATE_POLLING_MS "climate_polling_ms"
#define CONF_CLIMATE_UART_TX_PIN "climate_uart_tx_pin"
#define CONF_CLIMATE_UART_RX_PIN "climate_uart_rx_pin"

#define CONF_MQTT_UNIQUE_ID "mqtt_unique_id"
#define CONF_MQTT_BROKER_URI "mqtt_broker_uri"
#define CONF_MQTT_USER "mqtt_user"
#define CONF_MQTT_PASSWORD "mqtt_password"

static const char* ConfigList[] = {
    CONF_WIFI_SSID,
    CONF_WIFI_PASSWORD,
    CONF_CLIMATE_TYPE,
    CONF_CLIMATE_POLLING_MS,
    CONF_CLIMATE_UART_TX_PIN,
    CONF_CLIMATE_UART_RX_PIN,
    CONF_MQTT_UNIQUE_ID,
    CONF_MQTT_BROKER_URI,
    CONF_MQTT_USER,
    CONF_MQTT_PASSWORD,
    NULL
};