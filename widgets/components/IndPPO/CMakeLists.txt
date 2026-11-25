cmake_minimum_required(VERSION 3.20)

project(IndPPO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndPPO main_IndPPO.cpp)

target_link_libraries(IndPPO PRIVATE X11::X11)
