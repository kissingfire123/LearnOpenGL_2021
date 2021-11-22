#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

/* ��װ�õ�shader������,����Ҫ�༭ */

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
private:
	GLuint Program;/*����ID*/

public:
	/*��������ȡshader�ļ���������ɫ��*/
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	/*����Program-ID�������uniform*/
	GLuint GetProgram() const;
	/*ʹ�ó���*/
	void Use();
};
/*��������ȡshader�ļ���������ɫ��*/
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	/*step 1.���ļ�·���л�ȡ����*/
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	/*��֤ifstream��������׳��쳣*/
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		/*���ļ�*/
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		/*��ȡ�ļ��Ļ������ݵ�����*/
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		/*�ر��ļ�*/
		vShaderFile.close();
		fShaderFile.close();
		/*ת������GLchar����*/
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure &)
	{
		std::cout << "ERROR::Shader::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const GLchar * vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();

	/*Step 2.����vertex-Shader*/
	GLuint vertex;
	GLint success;
	GLchar infoLog[512];

	/*������ɫ��*/
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	/*��ӡ�������(����еĻ�)*/
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR:SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*Step 3.����fragment-Shader*/
	GLuint fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1,&fShaderCode, NULL);
	glCompileShader(fragment);
	/*��ӡ�������(����еĻ�)*/
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*Step 4.������ɫ������*/
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	/* ��ӡ���Ӵ���(����еĻ�)*/
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	/*Step 5.ɾ����ɫ��������*/
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

/*����Program-ID�������uniform*/
GLuint Shader::GetProgram() const
{
	return this->Program;
}

/*����Ҫuse-shader-program����ѭ������*/
void Shader::Use()
{
	glUseProgram(this->Program);
}
#endif
