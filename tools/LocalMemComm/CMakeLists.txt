set(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

get_filename_component(CURRENT_DIR_NAME ${CURRENT_DIR} NAME)

file(GLOB_RECURSE SRC_FILES "${CURRENT_DIR}/src/*.cpp")

add_executable(${CURRENT_DIR_NAME} ${SRC_FILES} "src/test.cpp")

target_include_directories(${CURRENT_DIR_NAME} PRIVATE "${CURRENT_DIR}/include")