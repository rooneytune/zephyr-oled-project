cmake_minimum_required(VERSION 3.22)
# Determine the build type
if (NOT BUILD_TYPE)
  set(BUILD_TYPE debug)
endif()

# Print out build type
message(STATUS "Build type: ${BUILD_TYPE} 🚀")

set(BOARD esp32)
# set(SHIELD ssd1306_128x32)

find_package(Zephyr)

project(hello_world_blinky)

target_sources(app PRIVATE 
    src/main.c
    src/button.c
    )
