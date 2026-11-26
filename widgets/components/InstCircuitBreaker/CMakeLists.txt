cmake_minimum_required(VERSION 3.20)

project(InstCircuitBreaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstCircuitBreaker main_InstCircuitBreaker.cpp)

target_link_libraries(InstCircuitBreaker PRIVATE X11::X11)
