#pragma once

#include <cstring>
#include <string>
#include "mqtt/MqttClient.h"

class MqttHAClimate : public MqttClient 
{
public:
    MqttHAClimate(const std::string &broker_uri,
                    const std::string &username,
                    const std::string &password,
                    const std::string &base_topic,
                    const std::string &unique_id);

    void publish_state(float room_temp, float target_temp, const std::string &power, const std::string &mode, const std::string &fan_mode);

    // Callbacks to be implemented in your app
    void on_mode_command(const std::string& mode);
    void on_temp_command(float temperature);
    void on_fan_mode_command(const std::string& fan_mode);
    void on_power_command(const std::string& power);
    void on_swing_command(const std::string& swing);

protected:
    // Override from MqttClient
    void on_connected() override;

    void publish_autodiscovery();

private:
    std::string unique_id;
    std::string base_topic;
    std::string state_topic;
};