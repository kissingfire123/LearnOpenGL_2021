
include_directories(../openGL_Depends/include)
include_directories(.)

set(LIBS_PATH ../openGL_Depends/libs)


find_library(GLFW3_LIB glfw3.lib   ${LIBS_PATH})
find_library(GLEW_LIB  glew32s.lib ${LIBS_PATH})
find_library(SOIL_LIB  SOIL.lib    ${LIBS_PATH})

#use find_package() to find system's OpenGL
find_package( OpenGL REQUIRED )
message ("find opengl is:  ${OPENGL_gl_LIBRARY} ")

set(GLSL_CODE Vertex.glsl  Fragment.glsl)
set_source_files_properties(${GLSL_CODE} PROPERTIES HEADER_FILE_ONLY TRUE) 	
		
ADD_EXECUTABLE(${PROJECT_NAME}   "${PROJECT_NAME}.cpp"  ShaderClass.hpp  ${GLSL_CODE} )
 
target_link_libraries(${PROJECT_NAME}   ${GLFW3_LIB}  ${GLEW_LIB} ${SOIL_LIB} ${OPENGL_gl_LIBRARY})

