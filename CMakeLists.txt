# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

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