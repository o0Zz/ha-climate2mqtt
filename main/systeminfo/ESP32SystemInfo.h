#pragma once
#include "ISystemInfo.h"
#include <string>
#include "esp_system.h"
#include "esp_timer.h"
#include "version.h"
#include "esp_idf_version.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_chip_info.h"
#include "esp_heap_caps.h"

extern "C" uint32_t esp_clk_cpu_freq(void);
extern "C" size_t spi_flash_get_chip_size(void);
namespace systeminfo {

class ESP32SystemInfo: public systeminfo::ISystemInfo {
public:

    std::string getVersion() const override {
        return APP_VERSION;
    }

    std::string getMacAddress() const override {
        uint8_t mac[6] = {0};
        if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
            return "";
        }
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return std::string(macStr);
    }

    std::string getIPAddress() const override {
        // Placeholder implementation; actual implementation would retrieve IP from network interface
        return "";
    }

    std::string getHostname() const override {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0)
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif != nullptr) {
            const char *hostname = nullptr;
            if (esp_netif_get_hostname(netif, &hostname) == ESP_OK && hostname != nullptr) {
                return std::string(hostname);
            }
        }
#endif
        return "";
    }

    uint64_t getUptimeSeconds() const override {
        return esp_timer_get_time() / 1000000;
    }

    uint32_t getFreeHeapBytes() const override {
        return esp_get_free_heap_size();
    }

    uint32_t getTotalHeapBytes() const override {
        return static_cast<uint32_t>(heap_caps_get_total_size(MALLOC_CAP_8BIT));
    }

    std::string getChipModel() const override {
        esp_chip_info_t chip_info = {};
        esp_chip_info(&chip_info);
        switch (chip_info.model) {
            case CHIP_ESP32:
                return "ESP32";
#if defined(CHIP_ESP32S2)
            case CHIP_ESP32S2:
                return "ESP32-S2";
#endif
#if defined(CHIP_ESP32S3)
            case CHIP_ESP32S3:
                return "ESP32-S3";
#endif
#if defined(CHIP_ESP32C2)
            case CHIP_ESP32C2:
                return "ESP32-C2";
#endif
#if defined(CHIP_ESP32C3)
            case CHIP_ESP32C3:
                return "ESP32-C3";
#endif
#if defined(CHIP_ESP32C6)
            case CHIP_ESP32C6:
                return "ESP32-C6";
#endif
#if defined(CHIP_ESP32H2)
            case CHIP_ESP32H2:
                return "ESP32-H2";
#endif
            default:
                return "Unknown";
        }
    }

    uint32_t getCpuFrequencyMHz() const override {
        return esp_clk_cpu_freq() / 1000000;
    }

    uint8_t getCpuCoreCount() const override {
        esp_chip_info_t chip_info = {};
        esp_chip_info(&chip_info);
        return chip_info.cores;
    }
    std::string getChipRevision() const override {
        esp_chip_info_t chip_info = {};
        esp_chip_info(&chip_info);
        return std::to_string(chip_info.revision);
    }

    uint32_t getFlashSizeBytes() const override {
        return static_cast<uint32_t>(spi_flash_get_chip_size());
    }

};

} // namespace systeminfo