//ʹ��EBO�������˸�ƽ���ı��Σ�ÿ��������ɫ��һ
#if 0
#include <iostream>

//����GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//����GLFW
#include <GLFW/glfw3.h>

//�ص�����
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

//����ά��
const GLuint WIDTH = 800, HEIGHT = 600;

//��ɫ�� shader:��ɫ������GLSL(OpenGL Shading Language)
//������ɫ�� vertexShader
//����shader:ÿ��������в�ͬ��color
const char* vertexShaderSource = "#version 330 core \n"
"layout (location =0) in vec3 position; \n"
"layout (location =1) in vec3 color; \n"
"out vec3 ourColor;//��fragment-Shader���һ��color \n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position,1.0);\n"
"	ourColor = color;\n"
"}\n\0";
#if 0 //��ͨshader�����vertexColor��fragment-Shader
const char* vertexShaderSource = "#version 330 core \n"
"layout (location = 0) in vec3 position; \n "
"out vec4 vertexColor ;\n"
"void main() \n"
"{ \n"
"     gl_Position = vec4(position.x,position.y,position.z,1.0); \n"
"     vertexColor = vec4(1.0f,0.0f,0.0f,1.0f);\n"
"}  \n\0";//ע�⣺ĩβ��Ҫ���ǽ�����"\0"
#endif


//ƬԪ��ɫ�� ע��GLSL���Ե��ַ�����ת����ʾ��"\"��������Ҳ����
const char* fragmentShaderSource = "#version 330 core\n \
 in vec3 ourColor; \n\
 out vec4 color; \n\
 void main() \n\
 {\n\
	color = vec4(ourColor,1.0f) ;\n\
 }\n\0";
#if 0 //ʹ����uniform��shader
const char* fragmentShaderSource = "#version 330 core\n \
 out vec4 color; \n\
 uniform vec4 ourColor; \n\
 void main() \n\
 {\n\
	color = ourColor ;\n\
 }\n\0";
#endif
#if 0 //��ͨshader������vertex��out
const char* fragmentShaderSource = "#version 330 core\n \
 in  vec4 vertexColor ;\n\
 out vec4 color; \n\
 void main() \n\
 {\n\
 	color = vertexColor; \n\
 }\n\0";//ע�⣺ĩβ��Ҫ���ǽ�����"\0"
#endif

int main(int argc, char *argv[]) 
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	//��ʼ��glfw
	glfwInit();
	//����glfw��������
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//����openGL�汾v3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//����Ϊcore-model,����ģʽ
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// ����glfw����
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Shader_Learning", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	//���ô��ڵĻص�����
	glfwSetKeyCallback(window, key_callback);


	//��ʼ�� GLEW ,���κ�openGL-lib�ں�������ǰ�ȵ������
	//����glewExperimental������ֵΪGL_TRUE���������ģʽ,��GLEW�ڹ���OpenGL�ĺ���ָ��ʱ�����ʹ���ִ����ļ���
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//�����ӿ�  viewPort ����OpenGL�е�λ������ת��Ϊ�����Ļ����
	//ע�⣬�������OpenGL���귶ΧֻΪ-1��1�����������ʵ�Ͻ�(-1��1)��Χ�ڵ�����ӳ�䵽(0, 800)��(0, 600)
	GLint width, height;
	//ֱ�Ӵ�GLFW�л�ȡ�ġ���GLFW�л�ȡ�ӿڵ�ά��,��������Ϊ800*600��Ϊ�������ڸ�DPI����Ļ��Ҳ������ʾ
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);//�������õı�GLFW��ά�ȸ�С������viewPort֮����ʾ������


	//==================>> ����˳��ͽ��˳��һ�㶼���ã�VAO-->VBO-->EBO  
	//�����������(Vertex Array Object, VAO)
	//��Ⱦǰָ��OpenGL����ν��Ͷ�������
	GLuint VAO;
	glGenVertexArrays(1/*size_n*/, &VAO);
	glBindVertexArray(VAO);// ��VAO

	//����ƽ���ı��ζ��㣺(2�����������,ʹ��EBO���Ը���2������!)
	GLfloat vertices[] = {
		// λ��            //��ɫ
		-0.5f,-0.5f,0.0f,   1.0f,0.0f,0.0f, //  ��A:����
		 0.5f,-0.5f,0.0f,   0.0f,1.0f,0.0f, //  ��B:����
		 0.0f, 0.5f,0.0f,   0.0f,0.0f,1.0f, //  ��C:���� 
		 1.0f, 0.5f,0.0f,   0.4f,0.1f,0.5f, //  ��D:���� 
	};
	//���㻺�����(Vertex Buffer Objects, VBO),һ���Եķ���һ�������ݵ��Կ�
	GLuint VBO;
	glGenBuffers(1, &VBO); //����һ��VBO����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//���㻺�����Ļ���������GL_ARRAY_BUFFER
	//glBufferData ����֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
	//ע�⣺�Կ���ι������������,��3�֣�
		//  GL_STATIC_DRAW �����ݲ���򼸺�����ı�
		//	GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶ�
		//	GL_STREAM_DRAW ������ÿ�λ���ʱ����ı�
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//�����εĶ�������,ΪEBO׼��
	GLuint indices[] = {
		0,2,3,//��1�������� 
		0,1,3 //��2��������
	};
	//�����������(Element Buffer Object��EBO��Ҳ��Index Buffer Object��IBO),��VBO֮��,���ú�VBO��ͬ
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//����OpenGL����ν�����������(Ӧ�õ��������������)
#if 0 //��������ֻ��λ�õ����
	glVertexAttribPointer(0,//�������Ե�λ��ֵ,shader��location 
		3,                   //�������ԵĴ�С,vec3
		GL_FLOAT,            //�������ͺ�
		GL_FALSE,            //�Ƿ��������ݱ�׼����[0,1](unsigned)����[-1,-1](signed)
		3 * sizeof(GLfloat),   //stride�����������Ķ���������֮��ļ������д0������openGL����
		(GLvoid*)NULL);      //λ�������ڻ�������ʼλ�õ�ƫ����(Offset)
	glEnableVertexAttribArray(0);//�������Ե�λ��ֵindex
#endif
#if 1// ����������positon��color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)NULL);
	glEnableVertexAttribArray(0);//��2������positon
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);//��2������color
#endif
	glBindVertexArray(0);//���VAO,�����������ط�������������ǡ�
	glBindBuffer(GL_ARRAY_BUFFER, 0); //���VBO 
	//VAO�ᴢ��glBindBuffer�ĺ�������,�������������,��Ҫ�ڽ��VAO֮ǰ���EBO,��������û�����EBO������,����OpenGL����ͼ
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//���EBO��ע�⣺EBO��󲻿�����VAO���֮ǰ



	//����һ��vertex-Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//�������ɫ��Դ�븽�ӵ���ɫ��������
	glShaderSource(vertexShader, 1/*Դ���ַ�������*/, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);//�������shader
	//���shader�ı����Ƿ�ɹ��������񱨴���Ϣ
	GLint success;
	GLchar infoLog[512] = { 0 };
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL/*ָ��int��log�ַ�������*/, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//����һ��fragment shader (���̺�Vertex-shader����)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL/*ָ��int��log�ַ�������*/, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//����Shaders:������ɫ���������ӵ�һ��������Ⱦ����ɫ������(Shader Program)��
	GLuint shaderProgram = glCreateProgram();
	//��֮ǰ�������ɫ�����ӵ����������
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//ͬ���أ��Ƿ����ӳɹ������Բ�ѯ��״̬�Լ�infoLog
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//shader���ӵ���������Ժ�,������Ҫ����,�ǵ�ɾ��
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//����ѭ��/�¼�ѭ��
	while (!glfwWindowShouldClose(window))
	{
		//��ѭ���ʼ�������û�д���ʲô�¼�������������롢����ƶ��ȣ�
		//Ȼ����ö�Ӧ�Ļص�����������ͨ���ص������ֶ����ã�
		glfwPollEvents();

		//��Ⱦ����
		//step1: ���õ�ɫ����
		glClearColor(0.4f, 0.1f, 0.5f, 0.5F);//�ĸ�����RGBA,��Χ����[0.0,1.0],(0.4,0.1,0.5)����Ubuntu����ɫ
		glClear(GL_COLOR_BUFFER_BIT);
		//step2: ����Shader-program
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);//��VAO
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		
#if 0
		//����shader�����uniform����
		GLfloat timeValue = glfwGetTime();//��ȡ���е�����
		GLfloat redValue = (sin(timeValue) / 2) + 0.5;//�õ�һ��[0.0,1.0]��Χ�ĸ�����
		GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");//Shader������Unfiorm������
#endif
		glUseProgram(shaderProgram);
#if 0
		glUniform4f(vertexColorLocation, redValue, redValue, 0.0f, 1.0f);
#endif
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)NULL);//���һ������:ָ��EBO�е�ƫ���������ߴ���һ���������飩
		glBindVertexArray(0);//���VAO

		//��Ⱦ�����������Դ���ص�buffer������ˢ����ʾ����
		glfwSwapBuffers(window);
	}
	//�ͷ�VAO,VBO,EBO�������Դ
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// �ر�glfw���ں������Ӧ����Դ
	glfwTerminate();
	return 0;
}


//�򵥹��ܣ�����"ESC"�����˳�����
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

#endif