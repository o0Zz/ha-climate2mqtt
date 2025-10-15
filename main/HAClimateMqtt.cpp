#include "HAClimateMqtt.h"

#define TAG "HAClimateMqtt"

HAClimateMqtt::HAClimateMqtt(const char* broker_uri,
                             const char* client_id,
                             const char* base_topic,
                             const char* username,
                             const char* password)
    : base_topic(base_topic)
    , username(username ? username : "")
    , password(password ? password : "")
{
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = broker_uri;
    mqtt_cfg.credentials.client_id = client_id;
    if (!this->username.empty())
        mqtt_cfg.credentials.username = this->username.c_str();
    if (!this->password.empty()) 
        mqtt_cfg.credentials.authentication.password = this->password.c_str();

    client = esp_mqtt_client_init(&mqtt_cfg);

    // Compose Home Assistant topics
    mode_cmd_topic      = base_topic + std::string("/set/mode");
    temp_cmd_topic      = base_topic + std::string("/set/temperature");
    power_cmd_topic     = base_topic + std::string("/set/power");
    fan_cmd_topic       = base_topic + std::string("/set/fan_mode");
    swing_cmd_topic     = base_topic + std::string("/set/swing");
    state_topic         = base_topic + std::string("/state");
}

void HAClimateMqtt::start() 
{
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, HAClimateMqtt::mqtt_event_handler, this);
    esp_mqtt_client_start(client);
}

void HAClimateMqtt::stop() 
{
    esp_mqtt_client_stop(client);
    esp_mqtt_client_unregister_event(client, MQTT_EVENT_ANY, HAClimateMqtt::mqtt_event_handler);
    esp_mqtt_client_destroy(client);
}

void HAClimateMqtt::subscribe_topics() 
{
    esp_mqtt_client_subscribe(client, mode_cmd_topic.c_str(), 1);
    esp_mqtt_client_subscribe(client, temp_cmd_topic.c_str(), 1);
    esp_mqtt_client_subscribe(client, fan_mode_cmd_topic.c_str(), 1);
}

void HAClimateMqtt::ha_autodiscovery()
{
    char payload[512];
    snprintf(payload, sizeof(payload),
    "{"
        "\"name\":\"ESP32 AC\","
        "\"unique_id\":\"esp32_ac_1\","

            //Device
        "\"device\":{"
            "\"identifiers\":[\"esp32_ac_1\"],"
            "\"name\":\"ESP32 AC Unit\","
            "\"model\":\"ESP32 Climate Controller\","
            "\"manufacturer\":\"Espressif\""
        "},"

            //Mode
        "\"modes\":[\"off\",\"cool\",\"heat\",\"dry\",\"fan_only\"],"
        "\"mode_command_topic\":\"%s/set/mode\","
        "\"mode_state_topic\":\"%s/state\","
        "\"mode_template\":\"{{ value_json.mode if (value_json is defined and value_json.mode is defined and value_json.mode|length) else 'off' }}\","

            //Temperature
        "\"temperature_command_topic\":\"%s/set/temperature\","
        "\"temperature_state_topic\":\"%s/state\","
        "\"temperature_template\":\"{% if (value_json is defined and value_json.temperature is defined) %}{% if (value_json.temperature|int >= 16 and value_json.temperature|int <= 30) %}{{ value_json.temperature }}{% elif (value_json.temperature|int < 16) %}16{% elif (value_json.temperature|int > 30) %}30{% endif %}{% else %}22{% endif %}\","
        "\"current_temperature_topic\":\"%s/state\","
        "\"current_temperature_template\":\"{{ value_json.room_temperature if (value_json is defined and value_json.room_temperature is defined and value_json.room_temperature|int > 1) }}\","
        "\"min_temp\":16,\"max_temp\":30,\"temp_step\":0.5,"
        "\"temperature_unit\":\"C\","

            //Power
        "\"power_command_topic\":\"%s/set/power\","
        "\"power_state_topic\":\"%s/state\","
        "\"power_template\":\"{{ value_json.power if (value_json is defined and value_json.power is defined and value_json.power|length) else 'off' }}\","
        
            //Fan
        "\"fan_modes\":[\"auto\",\"low\",\"medium\",\"high\"],"
        "\"fan_mode_command_topic\":\"%s/set/fan_mode\","
        "\"fan_mode_state_topic\":\"%s/state\","
        "\"fan_mode_template\":\"{{ value_json.fan if (value_json is defined and value_json.fan is defined and value_json.fan|length) else 'auto' }}\","

            //Swing
        "\"swing_modes\":[\"AUTO\",\"1\",\"2\",\"3\",\"4\",\"5\",\"SWING\"],"
        "\"swing_mode_command_topic\":\"%s/set/swing\","
        "\"swing_mode_state_topic\":\"%s/state\","
        "\"swing_mode_template\":\"{{ value_json.swing if (value_json is defined and value_json.swing is defined and value_json.swing|length) else 'AUTO' }}\","

        //Action
        "\"action_topic\":\"%s/state\","
        "\"action_template\":\"{{ value_json.action if (value_json is defined and value_json.action is defined and value_json.action|length) else 'idle' }}\""
    "}");

    std::string discovery_topic = "homeassistant/climate/esp32_ac_1/config";
    esp_mqtt_client_publish(client, discovery_topic.c_str(), payload, 0, 1, true);
}

// This should be called whenever AC state changes
void HAClimateMqtt::publish_state(float current_temp, float target_temp,
                                  const char* mode, const char* fan_mode) {
    char payload[256];
    snprintf(payload, sizeof(payload),
        "{\"temperature\":%.1f,\"target_temperature\":%.1f,\"mode\":\"%s\",\"fan_mode\":\"%s\"}",
        current_temp, target_temp, mode, fan_mode);
    esp_mqtt_client_publish(client, state_topic.c_str(), payload, 0, 1, false);
}

// This is called from static event handler
void HAClimateMqtt::handle_command(const char* topic, const char* payload) {
    if (strcmp(topic, mode_cmd_topic.c_str()) == 0) {
        on_mode_command(payload);
    } else if (strcmp(topic, temp_cmd_topic.c_str()) == 0) {
        float temp = atof(payload);
        on_temp_command(temp);
    } else if (strcmp(topic, fan_mode_cmd_topic.c_str()) == 0) {
        on_fan_mode_command(payload);
    }
}

// These should be implemented in your main code
void HAClimateMqtt::on_mode_command(const std::string& mode) {
    ESP_LOGI(TAG, "Mode command received: %s", mode.c_str());
    // TODO: Implement control of AC hardware
}
void HAClimateMqtt::on_temp_command(float temperature) {
    ESP_LOGI(TAG, "Temperature command received: %.1f", temperature);
    // TODO: Implement control of AC hardware
}
void HAClimateMqtt::on_fan_mode_command(const std::string& fan_mode) {
    ESP_LOGI(TAG, "Fan mode command received: %s", fan_mode.c_str());
    // TODO: Implement control of AC hardware
}

void HAClimateMqtt::mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) 
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    HAClimateMqtt *instance = (HAClimateMqtt *)handler_args;

    if (event->event_id == MQTT_EVENT_CONNECTED) 
    {
        ESP_LOGI(TAG, "MQTT Connected");
        if (instance) 
            instance->subscribe_topics();
    } 
    else if (event->event_id == MQTT_EVENT_DATA) 
    {
        char topic[event->topic_len + 1];
        char payload[event->data_len + 1];
        memcpy(topic, event->topic, event->topic_len);
        topic[event->topic_len] = 0;
        memcpy(payload, event->data, event->data_len);
        payload[event->data_len] = 0;

        if (instance) 
            instance->handle_command(topic, payload);
    }
}