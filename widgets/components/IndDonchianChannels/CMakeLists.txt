cmake_minimum_required(VERSION 3.20)

project(IndDonchianChannels LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndDonchianChannels main_IndDonchianChannels.cpp)

target_link_libraries(IndDonchianChannels PRIVATE X11::X11)
