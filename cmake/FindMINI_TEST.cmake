

message( "finding mini-test!"  )

    set(MINI_TEST_PATH $ENV{MINI_TEST_PATH})
    if( MINI_TEST_PATH )

        message("Find MINI_TEST_PATH env!")
        message(${MINI_TEST_PATH})

        find_path( MINI_TEST_INCLUDE_DIR dbg.hpp "${MINI_TEST_PATH}/include" )

        if( MINI_TEST_INCLUDE_DIR )

            set( MINI_TEST_FOUND TRUE )

        else()

            set( MINI_TEST_FOUND FALSE )

        endif()

    else()

        if(NOT DEFINED DEP_PATH)
            set(DEP_PATH "${CMAKE_SOURCE_DIR}/deps")
        endif()

        if(NOT EXISTS ${DEP_PATH})
            file(MAKE_DIRECTORY ${DEP_PATH})
            message(STATUS "Created directory: ${DEP_PATH}")
        endif()

        set(MINI_TEST_PATH "${DEP_PATH}/mt")

        find_path( MINI_TEST_INCLUDE_DIR dbg.hpp "${MINI_TEST_PATH}/include" )

        if( MINI_TEST_INCLUDE_DIR )

            set( MINI_TEST_FOUND TRUE )

        else()

            message("Not Find MINI_TEST_PATH env,to clone...")

            execute_process(
                COMMAND git clone git@github.com:wu1274704958/mini-test.git mt
                WORKING_DIRECTORY ${DEP_PATH}
                RESULT_VARIABLE result
            )
            if(NOT result EQUAL 0)
                message(FATAL_ERROR "Failed to clone mini_test repository ${result}")
            else()
                message(STATUS "Successfully cloned mini_test repository.")
                find_path( MINI_TEST_INCLUDE_DIR dbg.hpp "${MINI_TEST_PATH}/include" )
                if( MINI_TEST_INCLUDE_DIR )
                    set( MINI_TEST_FOUND TRUE )
                endif()
            endif()
        endif()

       
        

    endif()
message("................................................................")