cmake_minimum_required(VERSION 3.20)

project(InstPanelLampBlue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstPanelLampBlue main_InstPanelLampBlue.cpp)

target_link_libraries(InstPanelLampBlue PRIVATE X11::X11)
