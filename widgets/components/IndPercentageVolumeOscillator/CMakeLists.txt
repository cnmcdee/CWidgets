cmake_minimum_required(VERSION 3.20)

project(IndPercentageVolumeOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndPercentageVolumeOscillator main_IndPercentageVolumeOscillator.cpp)

target_link_libraries(IndPercentageVolumeOscillator PRIVATE X11::X11)
