#pragma once

#include <cstdint>
#include "led_strip.h"
#include "esp_timer.h"

namespace led {

class Led {
public:
    explicit Led(int gpio_pin);
    ~Led();

    void on(uint64_t blink_period_us = 0);
    void off();
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(uint8_t brightness);

private:
    bool init();
    void applyColor(uint8_t r, uint8_t g, uint8_t b);
    void startBlink(uint64_t period_us);
    void stopBlink();
    void handleBlink();
    static void blinkTimerCallback(void* arg);

    int m_gpio_pin;
    led_strip_handle_t m_strip;
    bool m_initialized;
    uint8_t m_color[3];
    uint8_t m_brightness;
    bool m_is_on;
    esp_timer_handle_t m_blink_timer;
    bool m_blink_on;
};

} // namespace led