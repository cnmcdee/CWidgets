cmake_minimum_required(VERSION 3.20)

project(IndSwenlinTradingOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndSwenlinTradingOscillator main_IndSwenlinTradingOscillator.cpp)

target_link_libraries(IndSwenlinTradingOscillator PRIVATE X11::X11)
