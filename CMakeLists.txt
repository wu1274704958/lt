cmake_minimum_required(VERSION 3.0.0)
project(lt VERSION 0.1.0)

if(MSVC)
    add_definitions("/std:c++17")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(MINI_TEST)

if(MINI_TEST_FOUND)
    include_directories(${MINI_TEST_INCLUDE_DIR})
endif()
add_executable(tit_s tit_s.cpp)

