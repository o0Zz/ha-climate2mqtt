#include "EspNVSConfig.h"

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
        return false;
    }
    return save();
}

int32_t EspNVSConfig::getInt32(const char* key, int32_t defaultValue) const
{
    int32_t value = 0;
    esp_err_t err = nvs_get_i32(mNvsHandle, key, &value);
    if (err != ESP_OK) {
        return defaultValue;
    }
    return value;
}

bool EspNVSConfig::setInt32(const char* key, int32_t value)
{
    esp_err_t err = nvs_set_i32(mNvsHandle, key, value);
    if (err != ESP_OK) {
        return false;
    }
    return save();
}

const std::string EspNVSConfig::getString(const char* key, const std::string &defaultValue) const
{
    char valueBuffer[256];
    size_t bufferSize = sizeof(valueBuffer);
    
    esp_err_t err = nvs_get_str(mNvsHandle, key, valueBuffer, &bufferSize);
    if (err != ESP_OK || bufferSize == 0) {
        return defaultValue;
    }
    
    return std::string(valueBuffer);
}

bool EspNVSConfig::setString(const char* key, const std::string &value)
{
    esp_err_t err = nvs_set_str(mNvsHandle, key, value.c_str());
    if (err != ESP_OK) {
        return false;
    }
    return commitChanges();
}

