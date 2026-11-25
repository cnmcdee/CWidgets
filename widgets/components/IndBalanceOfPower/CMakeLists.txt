cmake_minimum_required(VERSION 3.20)

project(IndBalanceOfPower LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndBalanceOfPower main_IndBalanceOfPower.cpp)

target_link_libraries(IndBalanceOfPower PRIVATE X11::X11)
