cmake_minimum_required(VERSION 3.20)

project(InstPanelLampWhite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstPanelLampWhite main_InstPanelLampWhite.cpp)

target_link_libraries(InstPanelLampWhite PRIVATE X11::X11)
