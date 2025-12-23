#pragma once
#include "IConfig.h"
#include "nvs_flash.h"
#include <unordered_map>
#include <string>

class EspNVSConfig : public IConfig {
public:
    EspNVSConfig();
    virtual ~EspNVSConfig() override;

    virtual bool load() override;
    virtual bool save() const override;
    virtual bool clear() override;

    virtual bool set(const char* key, const std::string &value) override;
    virtual bool set(std::map<std::string, std::string> keysValues) override;

    virtual int32_t getInt32(const char* key, int32_t defaultValue = 0) const override;
    virtual const std::string getString(const char* key, const std::string &defaultValue = "") const override;

    virtual std::map<std::string, std::string> getAll() const override;
    virtual std::map<std::string, const char* const*> getConfigValueList() const override;

private:
    nvs_handle_t mNvsHandle;
    mutable std::map<std::string, std::string> mCache;
};