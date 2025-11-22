#include "EspNVSConfig.h"
#include "esp_log.h"

#define TAG "EspNVSConfig"

EspNVSConfig::EspNVSConfig()
    : mNvsHandle(0)
{
    nvs_flash_init();
}

EspNVSConfig::~EspNVSConfig()
{
    if (mNvsHandle != 0) {
        nvs_close(mNvsHandle);
    }
}

bool EspNVSConfig::load()
{
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &mNvsHandle);
    return (err == ESP_OK);
}

bool EspNVSConfig::save() const
{
    esp_err_t err = nvs_commit(mNvsHandle);
    return (err == ESP_OK);
}

bool EspNVSConfig::clear()
{
    esp_err_t err = nvs_erase_all(mNvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase all NVS data: %s", esp_err_to_name(err));
        return false;
    }
    return save();
}

int32_t EspNVSConfig::getInt32(const char* key, int32_t defaultValue) const
{
    std::string value = getString(key, std::to_string(defaultValue));
    if (value.empty()) {
        //ESP_LOGE(TAG, "Failed to get int32 for key %s, returning default value %d", key, defaultValue);
        return defaultValue;
    }
    
    return atoi(value.c_str());
}   

bool EspNVSConfig::setInt32(const char* key, int32_t value)
{
    return setString(key, std::to_string(value));
}

const std::string EspNVSConfig::getString(const char* key, const std::string &defaultValue) const
{
    char valueBuffer[256];
    size_t bufferSize = sizeof(valueBuffer);
    
    esp_err_t err = nvs_get_str(mNvsHandle, key, valueBuffer, &bufferSize);
    if (err != ESP_OK || bufferSize == 0) {
        //ESP_LOGE(TAG, "Failed to get string for key %s: %s", key, esp_err_to_name(err));
        return defaultValue;
    }
    
    return std::string(valueBuffer);
}

bool EspNVSConfig::setString(const char* key, const std::string &value)
{
    esp_err_t err = nvs_set_str(mNvsHandle, key, value.c_str());
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set string for key %s: %s", key, esp_err_to_name(err));
        return false;
    }
    return save();
}

