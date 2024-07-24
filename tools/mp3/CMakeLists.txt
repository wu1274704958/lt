set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

get_filename_component(CURRENT_DIR_NAME ${CURRENT_DIR} NAME)

file(GLOB_RECURSE SRC_FILES "${CURRENT_DIR}/src/*.cpp")

add_library(${CURRENT_DIR_NAME} SHARED ${SRC_FILES})

target_include_directories(${CURRENT_DIR_NAME} PRIVATE "${CURRENT_DIR}/include")

target_compile_definitions(${CURRENT_DIR_NAME} PRIVATE EQD_MP_EXPORTS=1)

find_package(bass24)
find_package(bassflac24)

if(MINI_TEST_FOUND)
	target_include_directories(${CURRENT_DIR_NAME} PRIVATE ${MINI_TEST_INCLUDE_DIR})
endif()

target_link_libraries(${CURRENT_DIR_NAME} bass bassflac)

if(WIN32)
	target_compile_definitions(${CURRENT_DIR_NAME} PRIVATE PF_WIN32=1)
endif()