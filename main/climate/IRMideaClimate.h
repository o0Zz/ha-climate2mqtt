#pragma once

#include "climate/IoHubClimate.h"
#include "heatpump/iohub_heatpump_midea.h"

class IRMideaClimate : public IoHubClimate
{
public:
    IRMideaClimate(u8 txPin, u8 rxPin);
    ~IRMideaClimate() override;

    bool setup() override;
    bool refresh() override;

private:
    heatpump_midea heatpumpCtx;
    digital_async_receiver receiverCtx;

    u8 txPin;
    u8 rxPin;

    bool updateState() override;
};