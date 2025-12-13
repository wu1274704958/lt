add_library(libStatic STATIC libStatic.cpp)

add_library(libShared SHARED libShared.cpp)

target_compile_definitions(libShared PRIVATE -DEQD_MP_EXPORTS=)

add_dependencies(libShared libStatic)

target_link_libraries(libShared PUBLIC libStatic)

add_executable(testSharedLib main.cpp)

target_link_libraries(testSharedLib PRIVATE libShared)
 