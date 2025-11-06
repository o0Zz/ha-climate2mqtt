#include "mqtt/MqttClient.h"
#include "esp_log.h"
#include "esp_mac.h" // esp_base_mac_addr_get

#define TAG "MqttClient"

MqttClient::MqttClient(const std::string &broker_uri,
                                     const std::string &username,
                                     const std::string &password)
    : broker_uri(broker_uri),
      username(username),
      password(password)
{
    uint8_t mac[6];
    strcpy(client_id, "esp32_unknown_mac");
    if (esp_base_mac_addr_get(mac) == ESP_OK)
        snprintf(client_id, sizeof(client_id), "esp32_%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = this->broker_uri.c_str();
    mqtt_cfg.credentials.client_id = this->client_id;
    if (!this->username.empty())
        mqtt_cfg.credentials.username = this->username.c_str();
    if (!this->password.empty()) 
        mqtt_cfg.credentials.authentication.password = this->password.c_str();

    client = esp_mqtt_client_init(&mqtt_cfg);
    
    ESP_LOGI(TAG, "MQTT client created with client_id: %s", client_id);
}

MqttClient::~MqttClient()
{
    if (client) 
        esp_mqtt_client_destroy(client); //Cannot be called from the MQTT event handler
}

void MqttClient::start() 
{
    ESP_LOGD(TAG, "MQTT client starting ...");

    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, MqttClient::mqtt_event_handler, this);
    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "MQTT client started with client_id: %s", client_id);
}

void MqttClient::stop() 
{
    ESP_LOGD(TAG, "MQTT client stopping ...");

    esp_mqtt_client_stop(client); //Cannot be called from the MQTT event handler
    esp_mqtt_client_unregister_event(client, MQTT_EVENT_ANY, MqttClient::mqtt_event_handler);

    ESP_LOGI(TAG, "MQTT client stopped !");
}

bool MqttClient::subscribe(const std::string& topic, CommandCallback callback)
{
    topic_callbacks[topic] = callback;

    if (!client || !m_connected)
    {
        ESP_LOGW(TAG, "MQTT client not connected, cannot subscribe to topic: %s", topic.c_str());
        return false; // Not connected yet
    }

    int ret = esp_mqtt_client_subscribe(client, topic.c_str(), 1);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to subscribe to topic: %s", topic.c_str());
        return false;
    }

    ESP_LOGI(TAG, "Subscribed to topic: %s", topic.c_str());
    return true;
}

bool MqttClient::publish(const std::string& topic, const std::string &payload, int qos, int retain)
{
    if (!client || !m_connected)
    {
        ESP_LOGW(TAG, "MQTT client not connected, cannot publish to topic: %s", topic.c_str());
        return false;
    }

    ESP_LOGI(TAG, "Publishing to topic: %s, payload: %.255s...", topic.c_str(), payload.c_str());
    int ret = esp_mqtt_client_publish(client, topic.c_str(), payload.c_str(), 0, qos, retain);
    return (ret == ESP_OK);
}

void MqttClient::handle_command(const std::string& topic, const std::string& payload)
{
    auto it = topic_callbacks.find(topic);
    if (it == topic_callbacks.end()) {
        ESP_LOGW(TAG, "No callback registered for topic: %s", topic.c_str());
        return;
    }

    it->second(payload);
}

void MqttClient::mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) 
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    MqttClient *instance = (MqttClient *)handler_args;

    if (event->event_id == MQTT_EVENT_CONNECTED) 
    {
        ESP_LOGI(TAG, "MQTT Connected");
        
        // Subscribe to all registered topics
        for (const auto& pair : instance->topic_callbacks) {
            esp_mqtt_client_subscribe(instance->client, pair.first.c_str(), 1);
            ESP_LOGI(TAG, "Subscribed to topic: %s", pair.first.c_str());
        }

        instance->m_connected = true;
        instance->on_connected();
    }
    else if (event->event_id == MQTT_EVENT_DISCONNECTED) 
    {
        ESP_LOGI(TAG, "MQTT Disconnected");
        instance->m_connected = false;
        instance->on_disconnected();
    }
    else if (event->event_id == MQTT_EVENT_DATA) 
    {
        std::string topic(event->topic, event->topic_len);
        std::string payload(event->data, event->data_len);
        
        ESP_LOGI(TAG, "MQTT Data Received: topic=%s, data=%s",
                 topic.c_str(),
                 payload.c_str());

        instance->handle_command(topic, payload);
    }
}