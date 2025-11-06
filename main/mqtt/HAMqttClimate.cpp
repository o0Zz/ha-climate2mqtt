#include "mqtt/HAMqttClimate.h"
#include <json/nlohmann/json.hpp>
#include "esp_log.h"

using json = nlohmann::json;

#define TAG "MqttHAClimate"

const char *kModeStr[] = {
    "off",
    "heat",
    "cool",
    "auto",
    "dry",
    "fan_only"
};

const char *kFanModeStr[] = {
    "auto",
    "diffuse",
    "low",
    "medium",
    "high"
};

const char *kActionStr[] = {
    "idle",
    "heating",
    "cooling",
    "drying",
    "fan_only"
};

const char *kSwingStr[] = {
    "AUTO",
    "1",
    "2",
    "3",
    "4",
    "5",
    "SWING"
};

int find_enum_value(const std::string &value, const char* str_array[], size_t array_size) {
    for (size_t i = 0; i < array_size; ++i) {
        if (value == str_array[i]) {
            return static_cast<int>(i);
        }
    }
    return 0; // Default to first value if not found
}

HAMqttClimate::HAMqttClimate(const std::string &broker_uri,
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
        HAClimateMode mode = static_cast<HAClimateMode>(find_enum_value(payload, kModeStr, sizeof(kModeStr) / sizeof(kModeStr[0])));
        on_mode_command(mode);
    });

    subscribe(base_topic + "/set/temperature", [this](const std::string &payload) {
        float temp = atof(payload.c_str());
        on_temp_command(temp);
    });

    subscribe(base_topic + "/set/fan_mode", [this](const std::string &payload) {
        HAClimateFanMode fan_mode = static_cast<HAClimateFanMode>(find_enum_value(payload, kFanModeStr, sizeof(kFanModeStr) / sizeof(kFanModeStr[0])));
        on_fan_mode_command(fan_mode);
    });

    subscribe(base_topic + "/set/swing", [this](const std::string &payload) {
        HAClimateVaneMode vane = static_cast<HAClimateVaneMode>(find_enum_value(payload, kSwingStr, sizeof(kSwingStr) / sizeof(kSwingStr[0])));
        on_vane_command(vane);
    });

}

void HAMqttClimate::on_connected() 
{
    ESP_LOGI(TAG, "MQTT Connected !");
    publish_autodiscovery();
}

//https://www.home-assistant.io/integrations/mqtt/
bool HAMqttClimate::publish_autodiscovery()
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

    ESP_LOGI(TAG, "Publishing Home Assistant climate auto-discovery");
    return publish("homeassistant/climate/" + unique_id + "/config", j.dump(), 1, true);
}

// This should be called whenever AC state changes
bool HAMqttClimate::publish_state(float room_temp, 
                        float target_temp,
                        const HAClimateMode &mode, 
                        const HAClimateFanMode &fan_mode, 
                        const HAClimateVaneMode &vane_mode,
                        const HAClimateAction &action) 
{
    json j;
    j["room_temperature"] = room_temp;
    j["target_temperature"] = target_temp;
    j["mode"] = kModeStr[mode];
    j["fan_mode"] = kFanModeStr[fan_mode];
    j["swing_mode"] = kSwingStr[vane_mode];
    j["action"] = kActionStr[action];

    return publish(state_topic, j.dump(), 1, false);
}