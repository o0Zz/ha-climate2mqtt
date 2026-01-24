#pragma once

#include <string>

class LogBuffer
{
public:
    // Install log hooks for ESP-IDF and IOHub (optional). Safe to call once at startup.
    static void init();

    // Get current log snapshot (oldest -> newest).
    static std::string getSnapshot();

    // Clear stored logs.
    static void clear();
};
