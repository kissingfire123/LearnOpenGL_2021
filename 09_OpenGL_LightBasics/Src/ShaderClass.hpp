#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

/* 封装好的shader管理类,不需要编辑 */

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
private:
	GLuint Program;/*程序ID*/

public:
	/*构造器读取shader文件并构建着色器*/
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	/*给出Program-ID方便操作uniform*/
	GLuint GetProgram() const;
	/*使用程序*/
	void Use();
};
/*构造器读取shader文件并构建着色器*/
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	/*step 1.从文件路径中获取顶点*/
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	/*保证ifstream对象可以抛出异常*/
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		/*打开文件*/
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		/*读取文件的缓冲内容到流中*/
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		/*关闭文件*/
		vShaderFile.close();
		fShaderFile.close();
		/*转换流至GLchar数组*/
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure &)
	{
		std::cout << "ERROR::Shader::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const GLchar * vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();

	/*Step 2.编译vertex-Shader*/
	GLuint vertex;
	GLint success;
	GLchar infoLog[512];

	/*顶点着色器*/
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	/*打印编译错误(如果有的话)*/
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR:SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*Step 3.编译fragment-Shader*/
	GLuint fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1,&fShaderCode, NULL);
	glCompileShader(fragment);
	/*打印编译错误(如果有的话)*/
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*Step 4.创建着色器程序*/
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	/* 打印链接错误(如果有的话)*/
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	/*Step 5.删除着色器，清理*/
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

/*给出Program-ID方便操作uniform*/
GLuint Shader::GetProgram() const
{
	return this->Program;
}

/*在需要use-shader-program的主循环调用*/
void Shader::Use()
{
	glUseProgram(this->Program);
}
#endif
