#pragma once

#include "climate/IoHubClimate.h"
#include "heatpump/iohub_heatpump_midea.h"

class MideaClimate : public IoHubClimate
{
public:
    MideaClimate(u8 txPin, u8 rxPin);
    ~MideaClimate() override;

    bool setup() override;
    bool refresh() override;

private:
    heatpump_midea heatpumpCtx;
    digital_async_receiver receiverCtx;

    u8 txPin;
    u8 rxPin;

    bool updateState() override;
};