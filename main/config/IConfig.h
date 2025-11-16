#pragma once

#include <string>
#include <cstdint>

class IConfig {
public:
    virtual ~IConfig() = default;

    virtual bool load() = 0;
    virtual bool save() const = 0;
    virtual bool clear() = 0;

    virtual int32_t getInt32(const char* key, int32_t defaultValue = 0) const = 0;
    virtual bool setInt32(const char* key, int32_t value) = 0;

    virtual const std::string getString(const char* key, const std::string &defaultValue = "") const = 0;
    virtual bool setString(const char* key, const std::string &value) = 0;
};