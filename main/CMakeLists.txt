set(srcs "main.cpp" "HAClimateMqtt.cpp")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")
