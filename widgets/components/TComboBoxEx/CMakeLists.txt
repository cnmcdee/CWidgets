cmake_minimum_required(VERSION 3.20)

project(TComboBoxEx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TComboBoxEx main_TComboBoxEx.cpp)

target_link_libraries(TComboBoxEx PRIVATE X11::X11)
