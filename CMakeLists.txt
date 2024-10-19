cmake_minimum_required(VERSION 3.29)
project(ScreenMelt VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

add_executable(ScreenMelt WIN32 main.cpp)

target_compile_options(ScreenMelt PRIVATE $<$<CONFIG:Release>:-O3>)
target_link_options(ScreenMelt PRIVATE $<$<CONFIG:Release>:-static>)
