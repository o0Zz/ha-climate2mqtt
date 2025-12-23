#include "led/Led.h"
#include "esp_log.h"

namespace led {

namespace {
static const char* TAG = "Led";
}

Led::Led(int gpio_pin)
    : m_gpio_pin(gpio_pin)
    , m_strip(nullptr)
    , m_initialized(false)
    , m_color{255, 255, 255}
    , m_brightness(100)
    , m_is_on(false)
    , m_blink_timer(nullptr)
    , m_blink_on(false)
{
    init();
}

Led::~Led()
{
    stopBlink();

    if (m_strip != nullptr) {
        led_strip_clear(m_strip);
        led_strip_del(m_strip);
        m_strip = nullptr;
    }
}

void Led::on(uint64_t blink_period_us)
{
    if (!init()) {
        return;
    }

    m_is_on = true;

    if (blink_period_us == 0) {
        stopBlink();
        applyColor(m_color[0], m_color[1], m_color[2]);
        return;
    }

    startBlink(blink_period_us);
}

void Led::off()
{
    stopBlink();
    m_is_on = false;

    if (m_initialized) {
        applyColor(0, 0, 0);
    }
}

void Led::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;

    if (!m_is_on || !m_initialized) {
        return;
    }

    if (m_blink_timer == nullptr || m_blink_on) {
        applyColor(r, g, b);
    }
}

void Led::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;

    if (!m_is_on || !m_initialized) {
        return;
    }

    if (m_blink_timer == nullptr || m_blink_on) {
        applyColor(m_color[0], m_color[1], m_color[2]);
    }
}

bool Led::init()
{
    if (m_initialized) {
        return true;
    }

    led_strip_config_t strip_config = {};
    strip_config.strip_gpio_num = m_gpio_pin;
    strip_config.max_leds = 1;
    strip_config.led_model = LED_MODEL_WS2812;
    strip_config.led_pixel_format = LED_PIXEL_FORMAT_GRB;
    strip_config.flags.invert_out = false;

    led_strip_rmt_config_t rmt_config = {};
    rmt_config.clk_src = RMT_CLK_SRC_DEFAULT;
    rmt_config.resolution_hz = 10 * 1000 * 1000;
    rmt_config.mem_block_symbols = 64;
    rmt_config.flags.with_dma = false;

    esp_err_t ret = led_strip_new_rmt_device(&strip_config, &rmt_config, &m_strip);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create LED strip: %s", esp_err_to_name(ret));
        return false;
    }

    led_strip_clear(m_strip);
    m_initialized = true;
    ESP_LOGI(TAG, "LED initialized on GPIO %d", m_gpio_pin);
    return true;
}

void Led::applyColor(uint8_t r, uint8_t g, uint8_t b)
{
    if (!m_initialized || m_strip == nullptr) {
        return;
    }

    uint8_t scaled_r = static_cast<uint8_t>((static_cast<uint32_t>(r) * (m_brightness*2.55)) / 255);
    uint8_t scaled_g = static_cast<uint8_t>((static_cast<uint32_t>(g) * (m_brightness*2.55)) / 255);
    uint8_t scaled_b = static_cast<uint8_t>((static_cast<uint32_t>(b) * (m_brightness*2.55)) / 255);

    esp_err_t ret = led_strip_set_pixel(m_strip, 0, scaled_r, scaled_g, scaled_b);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set LED pixel: %s", esp_err_to_name(ret));
        return;
    }

    ret = led_strip_refresh(m_strip);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to refresh LED strip: %s", esp_err_to_name(ret));
    }
}

void Led::startBlink(uint64_t period_us)
{
    stopBlink();

    if (!m_initialized) {
        return;
    }

    esp_timer_create_args_t timer_args = {};
    timer_args.callback = &Led::blinkTimerCallback;
    timer_args.arg = this;
    timer_args.dispatch_method = ESP_TIMER_TASK;
    timer_args.name = "led_blink";
    timer_args.skip_unhandled_events = true;

    esp_err_t ret = esp_timer_create(&timer_args, &m_blink_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create blink timer: %s", esp_err_to_name(ret));
        applyColor(m_color[0], m_color[1], m_color[2]);
        return;
    }

    m_blink_on = true;
    applyColor(m_color[0], m_color[1], m_color[2]);

    ret = esp_timer_start_periodic(m_blink_timer, period_us);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start blink timer: %s", esp_err_to_name(ret));
        stopBlink();
    }
}

void Led::stopBlink()
{
    if (m_blink_timer != nullptr) {
        esp_timer_stop(m_blink_timer);
        esp_timer_delete(m_blink_timer);
        m_blink_timer = nullptr;
    }

    m_blink_on = false;
}

void Led::handleBlink()
{
    if (!m_initialized) {
        return;
    }

    m_blink_on = !m_blink_on;
    if (m_blink_on) {
        applyColor(m_color[0], m_color[1], m_color[2]);
    } else {
        applyColor(0, 0, 0);
    }
}

void Led::blinkTimerCallback(void* arg)
{
    auto* led = static_cast<Led*>(arg);
    if (led != nullptr) {
        led->handleBlink();
    }
}

} // namespace led
