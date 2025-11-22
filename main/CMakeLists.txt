#Include all .cpp files recursively in the main directory and subdirectories
file(GLOB_RECURSE srcs "*.cpp")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." REQUIRES libiohub driver esp_common esp_wifi esp_event nvs_flash mdns mqtt esp_http_server app_update)
