#Include all .cpp files in the main directory
file(GLOB srcs "*.cpp")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")
