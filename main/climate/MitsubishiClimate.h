#pragma once

#include "climate/IoHubClimate.h"
#include "heatpump/iohub_heatpump_mitsubishi.h"

class MitsubishiClimate : public IoHubClimate
{
public:
    MitsubishiClimate(u8 txPin, u8 rxPin);
    ~MitsubishiClimate() override;

    bool setup() override;
    bool refresh() override;

private:
    heatpump_mitsubishi heatpumpCtx;
    uart_ctx uartCtx;
    u8 txPin;
    u8 rxPin;

    bool updateState() override;
};