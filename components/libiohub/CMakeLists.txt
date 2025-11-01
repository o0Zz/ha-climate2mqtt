cmake_minimum_required(VERSION 3.5)

# This is an ESP-IDF component for libiohub
# It allows libiohub to be used as an ESP-IDF component with proper header access

# Set the platform for libiohub
set(IOHUB_PLATFORM "esp32" CACHE STRING "Select the IOHUB platform" FORCE)

# Convert to uppercase and lowercase for consistency
string(TOUPPER "${IOHUB_PLATFORM}" IOHUB_PLATFORM_UPPER)
string(TOLOWER "${IOHUB_PLATFORM}" IOHUB_PLATFORM_LOWER)

# Collect all common (platform-agnostic) sources
file(GLOB_RECURSE IOHUB_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libiohub/src/*.c
)

# Exclude platform-specific folders from the generic sources
list(FILTER IOHUB_SRC EXCLUDE REGEX ".*/platform/.*")

# Add platform-specific sources
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libiohub/src/platform/${IOHUB_PLATFORM_LOWER}")
    file(GLOB_RECURSE IOHUB_PLATFORM_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libiohub/src/platform/${IOHUB_PLATFORM_LOWER}/*.c
    )
    list(APPEND IOHUB_SRC ${IOHUB_PLATFORM_SRC})
else()
#    message(FATAL_ERROR "Unknown platform: ${IOHUB_PLATFORM} (no matching folder in ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libiohub/src/platform/)")
endif()

# Register as ESP-IDF component
idf_component_register(
    SRCS ${IOHUB_SRC}
    INCLUDE_DIRS "../../lib/libiohub/include"
    REQUIRES driver esp_common esp_timer
)

# Define platform macro for conditional compilation
target_compile_definitions(${COMPONENT_LIB}
    PUBLIC
        IOHUB_PLATFORM_${IOHUB_PLATFORM_UPPER}=1
        ESP_PLATFORM=1
)

# Set C standard
set_property(TARGET ${COMPONENT_LIB} PROPERTY C_STANDARD 11)
set_property(TARGET ${COMPONENT_LIB} PROPERTY C_STANDARD_REQUIRED ON)