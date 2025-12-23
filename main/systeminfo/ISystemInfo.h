#pragma once

#include <string>
#include <cstdint>

namespace systeminfo {

class ISystemInfo {
public:
    virtual ~ISystemInfo() = default;

    // System identification
    virtual std::string getVersion() const = 0;
    virtual std::string getMacAddress() const = 0;
    virtual std::string getIPAddress() const = 0;
    virtual std::string getHostname() const = 0;

    // System status
    virtual uint64_t getUptimeSeconds() const = 0;
    virtual uint32_t getFreeHeapBytes() const = 0;
    virtual uint32_t getTotalHeapBytes() const = 0;

    // System information
    virtual std::string getChipModel() const = 0;
    virtual uint32_t getCpuFrequencyMHz() const = 0;
    virtual uint8_t getCpuCoreCount() const = 0;
    virtual std::string getChipRevision() const = 0;
    
    // Flash information
    virtual uint32_t getFlashSizeBytes() const = 0;
   
};

} // namespace systeminfo