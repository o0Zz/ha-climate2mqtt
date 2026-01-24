#include "logs/LogBuffer.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_log.h"

#include "utils/iohub_logs.h"

namespace {
    constexpr size_t kLogBufferSize = 32 * 1024;

    static char s_logBuffer[kLogBufferSize];
    static size_t s_head = 0;
    static size_t s_tail = 0;
    static size_t s_used = 0;

    static portMUX_TYPE s_logMux = portMUX_INITIALIZER_UNLOCKED;

    static vprintf_like_t s_prevVprintf = nullptr;

    void logbuffer_append(const char *data, size_t len)
    {
        if (!data || len == 0) {
            return;
        }

        if (len >= kLogBufferSize) {
            data += (len - kLogBufferSize);
            len = kLogBufferSize;
        }

        portENTER_CRITICAL(&s_logMux);

        size_t freeSpace = kLogBufferSize - s_used;
        if (len > freeSpace) {
            size_t overflow = len - freeSpace;
            s_tail = (s_tail + overflow) % kLogBufferSize;
            s_used = kLogBufferSize;
        } else {
            s_used += len;
        }

        size_t firstChunk = std::min(len, kLogBufferSize - s_head);
        memcpy(s_logBuffer + s_head, data, firstChunk);
        if (len > firstChunk) {
            memcpy(s_logBuffer, data + firstChunk, len - firstChunk);
        }
        s_head = (s_head + len) % kLogBufferSize;

        portEXIT_CRITICAL(&s_logMux);
    }

    int logbuffer_vprintf(const char *fmt, va_list args)
    {
        char line[256];
        va_list args_copy;
        va_copy(args_copy, args);
        int len = vsnprintf(line, sizeof(line), fmt, args_copy);
        va_end(args_copy);
        if (len < 0) {
            len = 0;
        }

        bool truncated = false;
        if (static_cast<size_t>(len) >= sizeof(line)) {
            len = static_cast<int>(sizeof(line) - 1);
            truncated = true;
        }

        if (truncated && len > 0 && line[len - 1] != '\n') {
            line[len - 1] = '\n';
        }

        logbuffer_append(line, static_cast<size_t>(len));

        if (s_prevVprintf) {
            return s_prevVprintf(fmt, args);
        }
        return vprintf(fmt, args);
    }

}

void LogBuffer::init()
{
    if (s_prevVprintf == nullptr) {
        s_prevVprintf = esp_log_set_vprintf(logbuffer_vprintf);
    }
}

std::string LogBuffer::getSnapshot()
{
    std::string out;

    portENTER_CRITICAL(&s_logMux);

    out.resize(s_used);
    if (s_used > 0) {
        size_t firstChunk = std::min(s_used, kLogBufferSize - s_tail);
        memcpy(out.data(), s_logBuffer + s_tail, firstChunk);
        if (s_used > firstChunk) {
            memcpy(out.data() + firstChunk, s_logBuffer, s_used - firstChunk);
        }
    }

    portEXIT_CRITICAL(&s_logMux);

    return out;
}

void LogBuffer::clear()
{
    portENTER_CRITICAL(&s_logMux);
    s_head = 0;
    s_tail = 0;
    s_used = 0;
    portEXIT_CRITICAL(&s_logMux);
}
