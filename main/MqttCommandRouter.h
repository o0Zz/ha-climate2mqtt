#pragma once

#include <cstring>
#include <string>
#include <functional>
#include <map>
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

class MqttCommandRouter 
{
public:
    using CommandCallback = std::function<void(const std::string &payload)>;

    MqttCommandRouter(const std::string &broker_uri,
                      const std::string &username = "",
                      const std::string &password = "");
    
    virtual ~MqttCommandRouter();

    void start();
    void stop();
    
    // Subscribe to a topic with a callback function
    void subscribe(const std::string& topic, CommandCallback callback);
    
    // Publish a message to a topic
    void publish(const std::string& topic, const std::string &payload, int qos = 0, int retain = false);

protected:
    // Virtual method to be called when connected (for derived classes to subscribe to their topics)
    virtual void on_connected() {}
    
    // Handle incoming MQTT command - routes to registered callbacks
    void handle_command(const std::string &topic, const std::string &payload);

private:
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

    esp_mqtt_client_handle_t client;
    std::string broker_uri;
    std::string username;
    std::string password;
    char client_id[32];
    bool m_connected = false;
    
    // Map of topic to callback function
    std::map<std::string, CommandCallback> topic_callbacks;
};