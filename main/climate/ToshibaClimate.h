#pragma once

#include "climate/IoHubClimate.h"
#include "heatpump/iohub_heatpump_toshiba.h"

class ToshibaClimate : public IoHubClimate
{
public:
    ToshibaClimate(u8 txPin, u8 rxPin);
    ~ToshibaClimate() override;

    bool setup() override;
    bool refresh() override;

private:
    heatpump_toshiba_ctx heatpumpCtx;
    uart_ctx uartCtx;
    u8 txPin;
    u8 rxPin;

    bool updateState() override;
};