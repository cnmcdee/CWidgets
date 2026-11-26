cmake_minimum_required(VERSION 3.20)

project(InstPanelLampRed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstPanelLampRed main_InstPanelLampRed.cpp)

target_link_libraries(InstPanelLampRed PRIVATE X11::X11)
