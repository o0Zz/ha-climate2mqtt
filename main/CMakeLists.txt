#Include all .cpp files recursively in the main directory and subdirectories
file(GLOB_RECURSE srcs "*.cpp")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")
