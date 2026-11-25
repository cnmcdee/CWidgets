cmake_minimum_required(VERSION 3.20)

project(IndTwiggsMoneyFlow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndTwiggsMoneyFlow main_IndTwiggsMoneyFlow.cpp)

target_link_libraries(IndTwiggsMoneyFlow PRIVATE X11::X11)
