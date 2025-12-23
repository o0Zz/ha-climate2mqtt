#pragma once

#include <cstdint>
#include "led_strip.h"

/**
 * @brief Status LED states for visual indication
 */
enum class LedStatus {
    OFF,            // LED off
    CONNECTING,     // Blue - connecting to WiFi/MQTT/Climate
    OK,             // Green - everything working
    ERROR           // Red - connection error
};

/**
 * @brief RGB LED controller for ESP32-C6 onboard WS2812 LED
 * 
 * Controls the addressable RGB LED on GPIO8 to indicate system status:
 * - Blue: Connecting to WiFi/MQTT/Climate
 * - Green: All systems operational
 * - Red: Error state (connection failures)
 */
class StatusLed {
public:
    /**
     * @brief Construct a new Status Led object
     * @param gpio_pin GPIO pin number for the LED (default: 8 for ESP32-C6)
     */
    StatusLed(int gpio_pin = 8);
    
    ~StatusLed();

    /**
     * @brief Initialize the LED driver
     * @return true if initialization successful
     */
    bool setup();

    /**
     * @brief Set the LED status
     * @param status The status to display
     */
    void setStatus(LedStatus status);

    /**
     * @brief Get current LED status
     * @return Current LedStatus
     */
    LedStatus getStatus() const { return m_status; }

    /**
     * @brief Set custom RGB color
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Turn off the LED
     */
    void off();

    /**
     * @brief Set LED brightness (applied to all colors)
     * @param brightness Brightness level (0-255)
     */
    void setBrightness(uint8_t brightness);

private:
    int m_gpio_pin;
    led_strip_handle_t m_led_strip;
    LedStatus m_status;
    uint8_t m_brightness;
    bool m_initialized;

    void applyColor(uint8_t r, uint8_t g, uint8_t b);
};

/**
 * @brief Global status LED instance for easy access throughout the application
 */
extern StatusLed* g_status_led;

/**
 * @brief Initialize the global status LED
 * @param gpio_pin GPIO pin for the LED
 * @return true if successful
 */
bool status_led_init(int gpio_pin = 8);

/**
 * @brief Set status on the global LED
 * @param status Status to set
 */
void status_led_set(LedStatus status);
