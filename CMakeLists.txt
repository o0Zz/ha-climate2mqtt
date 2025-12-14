cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# ----------------------------------------------------------------------------
# Partition table selection
# ----------------------------------------------------------------------------
set(PARTITION_TABLE_CSV "${CMAKE_SOURCE_DIR}/partitions_no_ota.csv")
if(CONFIG_ENABLE_OTA)
    set(PARTITION_TABLE_CSV "${CMAKE_SOURCE_DIR}/partitions_ota.csv")
endif()
configure_file(${PARTITION_TABLE_CSV} ${CMAKE_SOURCE_DIR}/partitions.csv COPYONLY)

project(ha-climate2mqtt)

# ============================================================================
# Git Version Information
# ============================================================================
# Get version from git describe (uses tags like v1.0.0)
execute_process(
    COMMAND git describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

configure_file(
    ${CMAKE_SOURCE_DIR}/main/version.h.in
    ${CMAKE_SOURCE_DIR}/main/version.h
    @ONLY
)