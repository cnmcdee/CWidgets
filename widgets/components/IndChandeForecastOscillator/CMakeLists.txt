cmake_minimum_required(VERSION 3.20)

project(IndChandeForecastOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndChandeForecastOscillator main_IndChandeForecastOscillator.cpp)

target_link_libraries(IndChandeForecastOscillator PRIVATE X11::X11)
