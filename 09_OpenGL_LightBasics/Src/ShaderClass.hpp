#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

//To avoid text format and compile error, use English comment
/* No need to edit,a common shader file class,read->compile->link->use*/

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <cassert>

class Shader
{
private:
	GLuint Program;//Program-ID

public:
	// read shader file to construct  
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// get Program-ID to use uniform variable  
	GLuint GetProgram() const;
	 
	void Use();

    void SetVec3f(const GLchar *varName, const GLfloat* vec3fData);

    void SetMat4(const GLchar *varName, const GLfloat* mat4Data);
};
 
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath){
	/*step 1.read vertex data from file*/
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// make sure the ifstream can throw exception  
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try{
		vShaderFile.open(vertexPath);
        assert(vShaderFile.good());
		fShaderFile.open(fragmentPath);
        assert(fShaderFile.good());
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		// finally, convert the file content to char*
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure &){
		std::cout << "ERROR::Shader::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const GLchar * vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();

	/*Step 2.compile vertex-Shader*/
	GLuint vertex;
	GLint success;
	GLchar infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// if has compile error, get and print
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR:SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*Step 3.compile fragment-Shader*/
	GLuint fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1,&fShaderCode, NULL);
	glCompileShader(fragment);
    // if has compile error, get and print 
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*Step 4.Create shader program*/
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
    // if has link error, get and print  
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success){
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	/*Step 5.delete shader and clear resource*/
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

/*give Program-ID to operate uniform*/
GLuint Shader::GetProgram() const{
	return this->Program;
}

/*use the shader program in main-Loop*/
void Shader::Use(){
	glUseProgram(this->Program);
}

void Shader::SetVec3f(const GLchar *varName, const GLfloat* vec3fData) {
    GLint varLoc = glGetUniformLocation(this->Program, varName);
    glUniform3f(varLoc,  vec3fData[0], vec3fData[1], vec3fData[2]);
}

void Shader::SetMat4(const GLchar *varName, const GLfloat* mat4Data) {
    GLint varLoc = glGetUniformLocation(this->Program, varName);
    glUniformMatrix4fv(varLoc, 1, GL_FALSE, mat4Data);
}
#endif
