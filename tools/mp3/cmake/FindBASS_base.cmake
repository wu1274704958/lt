function(find_bass_base DepRootPath LibDir LibName)

if (WIN32)
	set(SYS_DIR "${DepRootPath}/win32")
else()

endif()

set(LibRootPath "${SYS_DIR}/${LibDir}")

if (NOT EXISTS "${LibRootPath}")
	message("Not found library ${LibDir}!")
else()

	set(include_path "${LibRootPath}/c")

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(library_path "${LibRootPath}/c/x64")
	else()
		set(library_path "${LibRootPath}/c")
	endif()

	find_path(${LibName}_INCLUDE_DIR "${LibName}.h" PATHS ${include_path})
	find_library(${LibName}_LIBRARY NAMES ${LibName} PATHS ${library_path})

	if(${LibName}_INCLUDE_DIR AND ${LibName}_LIBRARY )
		add_library(${LibName} STATIC IMPORTED)
        set_target_properties(${LibName} PROPERTIES
            IMPORTED_LOCATION ${${LibName}_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${${LibName}_INCLUDE_DIR}
        )
		message("find lib ${${LibName}_LIBRARY}")
	endif()

endif()

endfunction()