set(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

get_filename_component(CURRENT_DIR_NAME ${CURRENT_DIR} NAME)

file(GLOB_RECURSE SRC_FILES "${CURRENT_DIR}/src/*.cpp")

add_executable(${CURRENT_DIR_NAME} ${SRC_FILES} "test/test.cpp")

add_executable(test2 ${SRC_FILES} "test/test2.cpp")
add_executable(test3 ${SRC_FILES} "test/test3.cpp")

add_library(lmc SHARED ${SRC_FILES} "dll/lmc.cpp")

target_include_directories(${CURRENT_DIR_NAME} PRIVATE "${CURRENT_DIR}/include")
target_include_directories(lmc PRIVATE "${CURRENT_DIR}/include")

target_include_directories(test2 PRIVATE "${CURRENT_DIR}/include")
target_include_directories(test3 PRIVATE "${CURRENT_DIR}/include")

target_compile_definitions(lmc PRIVATE LMC_EXPORTS=1)