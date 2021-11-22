
include_directories(../../openGL_Depends/include)
include_directories(.)

#use find_package() to find system's OpenGL
find_package( OpenGL REQUIRED )
message ("find opengl is:  ${OPENGL_gl_LIBRARY} ")

set(GLSL_CODE Vertex.glsl  Fragment.glsl VertexLight.glsl  FragmentLight.glsl)
set_source_files_properties(${GLSL_CODE} PROPERTIES HEADER_FILE_ONLY TRUE) 	
		
ADD_EXECUTABLE(${PROJECT_NAME}   "${PROJECT_NAME}.cpp"  ShaderClass.hpp  CameraController.hpp ${GLSL_CODE} )

target_link_libraries(${PROJECT_NAME}    glfw  glew_s  SOIL ${OPENGL_gl_LIBRARY})

