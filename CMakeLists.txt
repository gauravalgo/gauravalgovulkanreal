cmake_minimum_required(VERSION 3.0)

project(vulkanreal)

add_executable(vulkanreal main.cpp)

install(TARGETS vulkanreal RUNTIME DESTINATION bin)
