cmake_minimum_required(VERSION 3.6.0)

PROJECT(OpenGL_CreateWindow)
ADD_SUBDIRECTORY(Src)

 if (MSVC)
	 # Set the startup project .
	if (NOT ${CMAKE_VERSION} VERSION_LESS "3.6.0")
		  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	endif ()
endif (MSVC)






	