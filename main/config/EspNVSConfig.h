#pragma once
#include "IConfig.h"
#include "nvs_flash.h"
#include <map>
#include <string>

class EspNVSConfig : public IConfig {
public:
    EspNVSConfig();
    virtual ~EspNVSConfig() override;

    virtual bool load() override;
    virtual bool save() const override;
    virtual bool clear() override;

    virtual int32_t getInt32(const char* key, int32_t defaultValue = 0) const override;
    virtual bool setInt32(const char* key, int32_t value) override;

    virtual const std::string getString(const char* key, const std::string &defaultValue = "") const override;
    virtual bool setString(const char* key, const std::string &value) override;
private:
    nvs_handle_t mNvsHandle;
    mutable std::map<std::string, std::string> mCache;
};