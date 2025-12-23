#include "led/StatusLed.h"
#include "esp_log.h"

static const char *TAG = "StatusLed";

// Color definitions (RGB values)
static const uint8_t COLOR_OFF[3]        = {0, 0, 0};
static const uint8_t COLOR_CONNECTING[3] = {0, 0, 255};    // Blue
static const uint8_t COLOR_OK[3]         = {0, 255, 0};    // Green
static const uint8_t COLOR_ERROR[3]      = {255, 0, 0};    // Red

// Global instance
StatusLed* g_status_led = nullptr;

StatusLed::StatusLed(int gpio_pin)
    : m_gpio_pin(gpio_pin)
    , m_led_strip(nullptr)
    , m_status(LedStatus::OFF)
    , m_brightness(25)  // Default to 10% brightness (25/255)
    , m_initialized(false)
{
}

StatusLed::~StatusLed()
{
    if (m_led_strip) {
        led_strip_clear(m_led_strip);
        led_strip_del(m_led_strip);
        m_led_strip = nullptr;
    }
}

bool StatusLed::setup()
{
    ESP_LOGI(TAG, "Initializing Status LED on GPIO %d", m_gpio_pin);

    // LED strip general initialization
    led_strip_config_t strip_config = {};
    strip_config.strip_gpio_num = m_gpio_pin;
    strip_config.max_leds = 1;  // Single LED on ESP32-C6
    strip_config.led_model = LED_MODEL_WS2812;
    strip_config.led_pixel_format = LED_PIXEL_FORMAT_GRB;
    strip_config.flags.invert_out = false;

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {};
    rmt_config.clk_src = RMT_CLK_SRC_DEFAULT;
    rmt_config.resolution_hz = 10 * 1000 * 1000; // 10MHz
    rmt_config.mem_block_symbols = 64;
    rmt_config.flags.with_dma = false;

    esp_err_t ret = led_strip_new_rmt_device(&strip_config, &rmt_config, &m_led_strip);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create LED strip: %s", esp_err_to_name(ret));
        return false;
    }

    // Clear LED on startup
    led_strip_clear(m_led_strip);

    m_initialized = true;
    ESP_LOGI(TAG, "Status LED initialized successfully");
    return true;
}

void StatusLed::setStatus(LedStatus status)
{
    if (!m_initialized) {
        ESP_LOGW(TAG, "LED not initialized");
        return;
    }

    m_status = status;

    switch (status) {
        case LedStatus::OFF:
            applyColor(COLOR_OFF[0], COLOR_OFF[1], COLOR_OFF[2]);
            ESP_LOGD(TAG, "LED status: OFF");
            break;
        case LedStatus::CONNECTING:
            applyColor(COLOR_CONNECTING[0], COLOR_CONNECTING[1], COLOR_CONNECTING[2]);
            ESP_LOGD(TAG, "LED status: CONNECTING (Blue)");
            break;
        case LedStatus::OK:
            applyColor(COLOR_OK[0], COLOR_OK[1], COLOR_OK[2]);
            ESP_LOGD(TAG, "LED status: OK (Green)");
            break;
        case LedStatus::ERROR:
            applyColor(COLOR_ERROR[0], COLOR_ERROR[1], COLOR_ERROR[2]);
            ESP_LOGD(TAG, "LED status: ERROR (Red)");
            break;
    }
}

void StatusLed::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    if (!m_initialized) {
        ESP_LOGW(TAG, "LED not initialized");
        return;
    }
    applyColor(r, g, b);
}

void StatusLed::off()
{
    setStatus(LedStatus::OFF);
}

void StatusLed::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
    // Re-apply current status with new brightness
    setStatus(m_status);
}

void StatusLed::applyColor(uint8_t r, uint8_t g, uint8_t b)
{
    if (!m_led_strip) {
        return;
    }

    // Apply brightness scaling
    uint8_t scaled_r = (r * m_brightness) / 255;
    uint8_t scaled_g = (g * m_brightness) / 255;
    uint8_t scaled_b = (b * m_brightness) / 255;

    esp_err_t ret = led_strip_set_pixel(m_led_strip, 0, scaled_r, scaled_g, scaled_b);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set LED pixel: %s", esp_err_to_name(ret));
        return;
    }

    ret = led_strip_refresh(m_led_strip);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to refresh LED strip: %s", esp_err_to_name(ret));
    }
}

// Global helper functions
bool status_led_init(int gpio_pin)
{
    if (g_status_led != nullptr) {
        ESP_LOGW(TAG, "Status LED already initialized");
        return true;
    }

    g_status_led = new StatusLed(gpio_pin);
    return g_status_led->setup();
}

void status_led_set(LedStatus status)
{
    if (g_status_led != nullptr) {
        g_status_led->setStatus(status);
    }
}
