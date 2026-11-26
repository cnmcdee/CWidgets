cmake_minimum_required(VERSION 3.20)

project(InstEmergencyStop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstEmergencyStop main_InstEmergencyStop.cpp)

target_link_libraries(InstEmergencyStop PRIVATE X11::X11)
