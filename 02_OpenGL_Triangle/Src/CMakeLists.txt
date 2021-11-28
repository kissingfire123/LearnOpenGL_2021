
include_directories(.)
#¿˚”√find_package√¸¡Ó’“µΩOpenGL
find_package( OpenGL REQUIRED )
message ("find opengl is:  ${OPENGL_gl_LIBRARY} ")

		
ADD_EXECUTABLE(${PROJECT_NAME}   "${PROJECT_NAME}.cpp")

target_link_libraries(${PROJECT_NAME}  glfw  glew_s ${OPENGL_gl_LIBRARY})

target_include_directories(${PROJECT_NAME} PUBLIC
                        $<TARGET_PROPERTY:glew_s,INCLUDE_DIRECTORIES>)
