#pragma once

#include <cstring>
#include <string>
#include "mqtt/MqttClient.h"
#include "climate/IClimate.h"

class HAMqttClimate : public MqttClient 
{
public:
    HAMqttClimate(const std::string &broker_uri,
                    const std::string &username,
                    const std::string &password,
                    const std::string &base_topic,
                    const std::string &unique_id);

    bool publish_state(float room_temp, 
                        float target_temp,
                        const HAClimateMode &mode, 
                        const HAClimateFanMode &fan_mode, 
                        const HAClimateVaneMode &vane_mode,
                        const HAClimateAction &action);

        // Interface to implement in your derived class
    virtual void on_mode_command(const HAClimateMode& mode) = 0;
    virtual void on_temp_command(float temperature) = 0;
    virtual void on_fan_mode_command(const HAClimateFanMode& fan_mode) = 0;
    virtual void on_vane_command(const HAClimateVaneMode& vane_mode) = 0;

protected:
    // Override from MqttClient
    void on_connected() override;

    bool publish_autodiscovery();

private:
    std::string unique_id;
    std::string base_topic;
    std::string state_topic;
};