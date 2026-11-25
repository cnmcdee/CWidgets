cmake_minimum_required(VERSION 3.20)

project(GtkAssistant LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkAssistant main_GtkAssistant.cpp)

target_link_libraries(GtkAssistant PRIVATE X11::X11)
