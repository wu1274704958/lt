

message( "finding chat_server!"  )

    set(CHAT_SERVER_PATH $ENV{CHAT_SERVER_PATH})
    if( CHAT_SERVER_PATH )

        message("Find CHAT_SERVER_PATH env!")
        message(${CHAT_SERVER_PATH})

        find_path( CHAT_SERVER_INCLUDE_DIR tools "${CHAT_SERVER_PATH}" NO_DEFAULT_PATH
            NO_PACKAGE_ROOT_PATH
            NO_CMAKE_PATH
            NO_CMAKE_ENVIRONMENT_PATH)

        if( CHAT_SERVER_INCLUDE_DIR )

            set( CHAT_SERVER_FOUND TRUE )

        else()

            set( CHAT_SERVER_FOUND FALSE )

        endif()

    else()

        set( CHAT_SERVER_FOUND FALSE )
        message("Not Find CHAT_SERVER_PATH env!")

    endif()
message("................................................................")