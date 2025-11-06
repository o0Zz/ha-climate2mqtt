#include "mqtt/HAMqttClimateImpl.h"

#define TAG "HAMqttClimateImpl"

HAMqttClimateImpl::HAMqttClimateImpl(std::shared_ptr<IClimate> climate,
					const std::string &broker_uri,
					 const std::string &username,
					 const std::string &password,
					 const std::string &base_topic,
					 const std::string &unique_id)
    : HAMqttClimate(broker_uri, username, password, base_topic, unique_id),
        climate(std::move(climate))
{
}

bool HAMqttClimateImpl::refresh()
{
    if (!climate->refresh())
        return false;

    return publish_state();
}

bool HAMqttClimateImpl::publish_state()
{
    return HAMqttClimate::publish_state(0.0,
                         climate->getTargetTemperature(),
                         climate->getMode(),
                         climate->getFanMode(),
                         climate->getVaneMode(),
                         HAClimateAction_Idle);
}

void HAMqttClimateImpl::on_mode_command(const HAClimateMode& mode) 
{
    climate->setMode(mode);
    publish_state();
}

void HAMqttClimateImpl::on_temp_command(float temperature)
{
    climate->setTargetTemperature(temperature);
    publish_state();
}

void HAMqttClimateImpl::on_fan_mode_command(const HAClimateFanMode& fan_mode) 
{
    climate->setFanMode(fan_mode);
    publish_state();
}

void HAMqttClimateImpl::on_vane_command(const HAClimateVaneMode& vane) 
{
    climate->setVaneMode(vane);
    publish_state();
}
