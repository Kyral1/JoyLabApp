cmake_minimum_required(VERSION 3.16)
set(COMPONENT_MANAGER_ENABLED ON CACHE BOOL "Enable IDF Component Manager")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)
project(ESP32JoyLab)


