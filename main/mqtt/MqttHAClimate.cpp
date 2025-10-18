#include "MqttHAClimate.h"
#include <json/nlohmann/json.hpp>
#include "esp_log.h"

using json = nlohmann::json;

#define TAG "MqttHAClimate"

MqttHAClimate::MqttHAClimate(const std::string &broker_uri,
                             const std::string &username,
                             const std::string &password,
                             const std::string &base_topic,
                             const std::string &unique_id)
    : MqttClient(broker_uri, username, password),
        unique_id(unique_id),
        base_topic(base_topic),
        state_topic(base_topic + "/state")
{
    // Register callbacks for each command topic
    subscribe(base_topic + "/set/mode", [this](const std::string &payload) {
        on_mode_command(payload);
    });

    subscribe(base_topic + "/set/temperature", [this](const std::string &payload) {
        float temp = atof(payload.c_str());
        on_temp_command(temp);
    });

    subscribe(base_topic + "/set/power", [this](const std::string &payload) {
        on_power_command(payload);
    });

    subscribe(base_topic + "/set/fan_mode", [this](const std::string &payload) {
        on_fan_mode_command(payload);
    });

    subscribe(base_topic + "/set/swing", [this](const std::string &payload) {
        on_swing_command(payload);
    });

}

void MqttHAClimate::on_connected() 
{
    ESP_LOGI(TAG, "MQTT Connected !");
    publish_autodiscovery();
}
//https://www.home-assistant.io/integrations/mqtt/
void MqttHAClimate::publish_autodiscovery()
{
    json j;

    // Basic info
    j["name"] = unique_id;
    j["unique_id"] = unique_id;

    // Device
    j["device"] = {
        {"identifiers", {unique_id}},
        {"name", "ESP32 AC"},
        {"model", "ESP32 Climate Controller"},
        {"manufacturer", "Espressif"}
    };

    // Modes
    j["modes"] = {"off", "cool", "heat", "dry", "fan_only"};
    j["mode_command_topic"] = base_topic + "/set/mode";
    j["mode_state_topic"]   = state_topic;
    j["mode_state_template"] = "{{ value_json.mode if (value_json is defined and value_json.mode is defined and value_json.mode|length) else 'off' }}";

    // Temperature
    j["temperature_command_topic"] = base_topic + "/set/temperature";
    j["temperature_state_topic"]   = state_topic;
    j["temperature_state_template"] =
        "{% if (value_json is defined and value_json.target_temperature is defined) %}"
        "{% if (value_json.target_temperature|int >= 16 and value_json.target_temperature|int <= 30) %}{{ value_json.target_temperature }}"
        "{% elif (value_json.target_temperature|int < 16) %}16"
        "{% elif (value_json.target_temperature|int > 30) %}30"
        "{% endif %}{% else %}22{% endif %}";
    j["current_temperature_topic"] = state_topic;
    j["current_temperature_template"] =
        "{{ value_json.room_temperature if (value_json is defined and value_json.room_temperature is defined and value_json.room_temperature|int > 1) }}";
    j["min_temp"] = 16;
    j["max_temp"] = 30;
    j["temp_step"] = 0.5;
    j["temperature_unit"] = "C";

    // Power
    j["power_command_topic"] = base_topic + "/set/power";
    //j["power_template"] = "{{ value_json.power if (value_json is defined and value_json.power is defined and value_json.power|length) else 'off' }}";

    // Fan
    j["fan_modes"] = {"auto", "diffuse" /*Quiet*/, "low", "medium", "high"};
    j["fan_mode_command_topic"] = base_topic + "/set/fan_mode";
    j["fan_mode_state_topic"]   = state_topic;
    j["fan_mode_state_template"] =
        "{{ value_json.fan_mode if (value_json is defined and value_json.fan_mode is defined and value_json.fan_mode|length) else 'auto' }}";

    // Swing
    j["swing_modes"] = {"AUTO", "1", "2", "3", "4", "5", "SWING"};
    j["swing_mode_command_topic"] = base_topic + "/set/swing";
    j["swing_mode_state_topic"]   = state_topic;
    j["swing_mode_state_template"] =
        "{{ value_json.swing if (value_json is defined and value_json.swing is defined and value_json.swing|length) else 'AUTO' }}";

    // Action
    j["action_topic"] = state_topic;
    j["action_template"] = "{{ value_json.action if (value_json is defined and value_json.action is defined and value_json.action|length) else 'idle' }}";

    publish("homeassistant/climate/" + unique_id + "/config", j.dump(), 1, true);
    
    ESP_LOGI(TAG, "Published Home Assistant climate auto-discovery");
}

// This should be called whenever AC state changes
void MqttHAClimate::publish_state(float room_temp, float target_temp, const std::string &power, const std::string &mode, const std::string &fan_mode) 
{
    char payload[512];
    snprintf(payload, sizeof(payload),
        "{\"room_temperature\":%.1f,\"target_temperature\":%.1f,\"mode\":\"%s\",\"fan_mode\":\"%s\", \"power\":\"%s\", \"swing\":\"AUTO\", \"action\":\"idle\"}",
        room_temp, target_temp, mode.c_str(), fan_mode.c_str(), power.c_str());
    publish(state_topic, payload, 1, false);
}

// These should be implemented in your main code I LOVE YOU 
void MqttHAClimate::on_mode_command(const std::string& mode) {
    ESP_LOGI(TAG, "Mode command received: %s", mode.c_str());
    // TODO: Implement control of AC hardware
}

void MqttHAClimate::on_temp_command(float temperature) {
    ESP_LOGI(TAG, "Temperature command received: %.1f", temperature);
    // TODO: Implement control of AC hardware
}

void MqttHAClimate::on_fan_mode_command(const std::string& fan_mode) {
    ESP_LOGI(TAG, "Fan mode command received: %s", fan_mode.c_str());
    // TODO: Implement control of AC hardware
}

void MqttHAClimate::on_power_command(const std::string& power) {
    ESP_LOGI(TAG, "Power command received: %s", power.c_str());
    // TODO: Implement control of AC hardware
}

void MqttHAClimate::on_swing_command(const std::string& swing) {
    ESP_LOGI(TAG, "Swing command received: %s", swing.c_str());
    // TODO: Implement control of AC hardware
}