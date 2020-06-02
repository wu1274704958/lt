cmake_minimum_required(VERSION 3.0.0)
project(lt VERSION 0.1.0)

if(MSVC)
    add_definitions("/std:c++17")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(MINI_TEST)
find_package(ZLIB)
find_package(CS)

if(MINI_TEST_FOUND)
    include_directories(${MINI_TEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
endif()

if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
    link_libraries(${ZLIB_LIBRARY})
endif()

if(CHAT_SERVER_FOUND)
    message("CHAT_SERVER_INCLUDE_DIR = ${CHAT_SERVER_INCLUDE_DIR}")
    include_directories(${CHAT_SERVER_INCLUDE_DIR})
endif()

set(CLIENT_IMPL_SRC ${CMAKE_SOURCE_DIR}/client/socket_win_impl.cpp)
set(CLIENT_IMPL_LIB ws2_32.lib)

Function(Exec ag1 )
    set(CLIENT_EXEC_NAME ${ag1})
    add_executable(${CLIENT_EXEC_NAME} "${CLIENT_EXEC_NAME}.cpp" ${CLIENT_IMPL_SRC})
    target_link_libraries(${CLIENT_EXEC_NAME} ${CLIENT_IMPL_LIB})
endfunction(Exec)

Exec(tit_s)

Exec(merge_val)
Exec(mv_r)
Exec(up_bv)
Exec(modify_json)
#onRPR onRequestPermissionsResult
Exec(link_onRPR)
Exec(swap_f)
Exec(link_onABC)
Exec(link_onBP)
Exec(test)
Exec(add_n_v2)
Exec(gen_n_v2)
Exec(down_for_aid)
Exec(down_v1v2)
Exec(down_res)
Exec(gen_res_conf)
Exec(replace_wxid)
Exec(plug)
Exec(rename)
