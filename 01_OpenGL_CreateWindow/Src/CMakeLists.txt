
include_directories(../openGL_Depends/include)

set(LIBS_PATH ../openGL_Depends/libs)


find_library(GLFW3_LIB glfw3.lib   ${LIBS_PATH})
find_library(GLEW_LIB  glew32s.lib ${LIBS_PATH})

#¿˚”√find_package√¸¡Ó’“µΩOpenGL
find_package( OpenGL REQUIRED )
message ("find opengl is:  ${OPENGL_gl_LIBRARY} ")

		
ADD_EXECUTABLE(${PROJECT_NAME}   "${PROJECT_NAME}.cpp")

target_link_libraries(${PROJECT_NAME}   ${GLFW3_LIB}  ${GLEW_LIB} ${OPENGL_gl_LIBRARY})
