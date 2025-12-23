#include "EspNVSConfig.h"
#include "ConfigConsts.h"
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

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS handle: %s", esp_err_to_name(err));
        return false;
    }

        // Ensure all default keys are present
    for (const auto& entry : ConfigList) {
        std::string value = getString(entry.first);
        if (value.empty() && entry.second != nullptr)
            set(entry.first, entry.second);
    }

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

int32_t EspNVSConfig::getInt32(const std::string &key, int32_t defaultValue) const
{
    std::string value = getString(key);
    if (value.empty()) {
        //ESP_LOGE(TAG, "Failed to get int32 for key %s, returning default value %d", key, defaultValue);
        return defaultValue;
    }
    
    return atoi(value.c_str());
}   

const std::string EspNVSConfig::getString(const std::string &key, const std::string &defaultValue) const
{
    char valueBuffer[256];
    size_t bufferSize = sizeof(valueBuffer);
    
    esp_err_t err = nvs_get_str(mNvsHandle, key.c_str(), valueBuffer, &bufferSize);
    if (err != ESP_OK || bufferSize == 0) {
        
        //ESP_LOGE(TAG, "Failed to get string for key %s: %s", key, esp_err_to_name(err));
        return defaultValue;
    }
    
    return std::string(valueBuffer);
}

bool EspNVSConfig::set(const std::string &key, const std::string &value)
{
    esp_err_t err = nvs_set_str(mNvsHandle, key.c_str(), value.c_str());
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set string for key %s: %s", key.c_str(), esp_err_to_name(err));
        return false;
    }
    return save();
}

bool EspNVSConfig::set(std::map<std::string, std::string> keysValues) 
{
    for (const auto& kv : keysValues) 
    {
        for (const auto& entry : ConfigList)
        {
            if (kv.first == entry.first) {
                ESP_LOGI(TAG, "Saving config key: %s value: %s", kv.first.c_str(), kv.second.c_str());
                if (!set(kv.first.c_str(), kv.second.c_str()))
                    return false;
            }
        }
    }

    return true;
}

std::map<std::string, std::string> EspNVSConfig::getAll() const
{
    std::map<std::string, std::string> allConfigs;
    for (const auto& entry : ConfigList) {
        const char* key = entry.first.c_str();
        std::string value = getString(key);
        allConfigs[key] = value;
    }
    return allConfigs;
}

std::map<std::string, const char* const*> EspNVSConfig::getConfigValueList() const
{
    return ConfigValueList;
}
