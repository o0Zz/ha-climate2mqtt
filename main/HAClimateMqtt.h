#pragma once

#include <cstring>
#include <string>
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

class HAClimateMqtt 
{
public:
    HAClimateMqtt(const char* broker_uri,
                  const char* client_id,
                  const char* base_topic,
                  const char* username = nullptr,
                  const char* password = nullptr);

    void start();
    void stop();

    void publish_state(float current_temp, float target_temp,
                       const char* mode, const char* fan_mode);

    // Callbacks to be implemented in your app
    void on_mode_command(const std::string& mode);
    void on_temp_command(float temperature);
    void on_fan_mode_command(const std::string& fan_mode);
    void on_power_command(const std::string& power);
    void on_swing_command(const std::string& swing);

private:
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

    void subscribe_topics();
    void handle_command(const char* topic, const char* payload);

    esp_mqtt_client_handle_t client;
    std::string base_topic;
    std::string mode_cmd_topic;
    std::string temp_cmd_topic;
    std::string state_topic;
    std::string power_cmd_topic;
    std::string fan_cmd_topic;
    std::string swing_cmd_topic;

    // Credentials
    std::string username;
    std::string password;
};