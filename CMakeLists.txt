cmake_minimum_required(VERSION 3.0.0)
project(lt VERSION 0.1.0)

if(MSVC)
    add_definitions("/std:c++17")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(MINI_TEST)

if(MINI_TEST_FOUND)
    include_directories(${MINI_TEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
endif()
add_executable(tit_s tit_s.cpp)
add_executable(merge_val merge_val.cpp)
add_executable(mv_r mv_r.cpp)
add_executable(up_bv up_bv.cpp)
add_executable(modify_json modify_json.cpp)
#onRPR onRequestPermissionsResult
add_executable(link_onRPR link_onRPR.cpp)
add_executable(swap_f swap_f.cpp)

