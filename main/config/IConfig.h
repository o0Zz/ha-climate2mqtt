#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <map>

class IConfig {
public:
    virtual ~IConfig() = default;

    virtual bool load() = 0;
    virtual bool save() const = 0;
    virtual bool clear() = 0;

    virtual bool set(const std::string &key, const std::string &value) = 0;
    virtual bool set(std::map<std::string, std::string> keysValues) = 0;

    virtual int32_t getInt32(const std::string &key, int32_t defaultValue = 0) const = 0;
    virtual const std::string getString(const std::string &key, const std::string &defaultValue = "") const = 0;
    
    virtual std::map<std::string, std::string> getAll() const = 0;
    virtual std::map<std::string, const char* const*> getConfigValueList() const = 0;
};