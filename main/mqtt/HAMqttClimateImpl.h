#pragma once

#include "climate/IClimate.h"
#include "mqtt/HAMqttClimate.h"
#include <memory>

class HAMqttClimateImpl : public HAMqttClimate 
{
public:
	HAMqttClimateImpl(std::shared_ptr<IClimate> climate,
					const std::string &broker_uri,
					 const std::string &username,
					 const std::string &password,
					 const std::string &base_topic,
					 const std::string &unique_id);
	
	bool refresh();

protected:
	// Interface to implement in your derived class
	void on_mode_command(const HAClimateMode& mode) override;
	void on_temp_command(float temperature) override;
	void on_fan_mode_command(const HAClimateFanMode& fan_mode) override;
	void on_vane_command(const HAClimateVaneMode& vane) override;

	bool publish_state();

private:
	std::shared_ptr<IClimate> climate;
	
};