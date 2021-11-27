cmake_minimum_required(VERSION 3.6.0)

cmake_policy(SET CMP0077 NEW)

PROJECT(OpenGL_LightBasics)

if (MSVC)
	set(GLFW_BUILD_WIN32 ON)
	 # Set the startup project .
	if (NOT ${CMAKE_VERSION} VERSION_LESS "3.6.0")
		  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	endif ()
endif (MSVC)

if(APPLE)
	set(GLFW_BUILD_COCOA ON)
endif(APPLE)

ADD_SUBDIRECTORY(Src)
ADD_SUBDIRECTORY(../Dependency/glm    ${CMAKE_BINARY_DIR}/Dependency/glm)
ADD_SUBDIRECTORY(../Dependency/glfw-3.3     ${CMAKE_BINARY_DIR}/Dependency/glfw)
ADD_SUBDIRECTORY(../Dependency/soil-0.1.1   ${CMAKE_BINARY_DIR}/Dependency/SOIL)
ADD_SUBDIRECTORY(../Dependency/glew-2.0.0/build/cmake  ${CMAKE_BINARY_DIR}/Dependency/glew)


	