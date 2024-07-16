
set(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB SUBDIRS RELATIVE ${CURRENT_DIR} ${CURRENT_DIR}/*)

foreach(subDir ${SUBDIRS})
	 if(IS_DIRECTORY ${CURRENT_DIR}/${subDir})
		add_subdirectory(${subDir})
	 endif()
endforeach()