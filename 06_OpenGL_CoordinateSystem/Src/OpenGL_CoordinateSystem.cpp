// OpenGL_CoordinateSystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>

//引入GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//引入GLFW
#include <GLFW/glfw3.h>

#include "ShaderClass.hpp"

//引入SOIL
#include "SOIL/SOIL.h"



//回调函数
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

//窗口维度
const GLuint WIDTH = 800, HEIGHT = 600;

//透明度调节
GLuint gTextureAlpha = 20;

int main(int argc, char *argv[]) {
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

	//初始化glfw
	glfwInit();
	//设置glfw窗口属性
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置openGL版本v3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//设置为core-model,核心模式
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// 创建glfw窗口
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Shader_Learning", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	//设置窗口的回调函数
	glfwSetKeyCallback(window, key_callback);


	//初始化 GLEW ,在任何openGL-lib内函数调用前先调用这个
	//设置glewExperimental变量的值为GL_TRUE，适配核心模式,让GLEW在管理OpenGL的函数指针时更多地使用现代化的技术
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}


	//设置视口  viewPort ：将OpenGL中的位置坐标转换为你的屏幕坐标
	//注意，处理过的OpenGL坐标范围只为-1到1，因此我们事实上将(-1到1)范围内的坐标映射到(0, 800)和(0, 600)
	GLint width, height;
	//直接从GLFW中获取的。从GLFW中获取视口的维度,而不设置为800*600是为了让它在高DPI的屏幕上也正常显示
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);//可以设置的比GLFW的维度更小，能在viewPort之外显示其他的




	//定义三角形顶点
	GLfloat vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
	};
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
	//顶点缓冲对象(Vertex Buffer Objects, VBO),一次性的发送一大批数据到显卡
	GLuint VBO;
	//顶点数组对象(Vertex Array Object, VAO)
	//渲染前指定OpenGL该如何解释顶点数据
	GLuint VAO;
	glGenVertexArrays(1/*size_n*/, &VAO);
	

	glGenBuffers(1, &VBO); //生成一个VBO对象
	
	GLuint EBO;
    glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);// 绑定VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER
	//glBufferData ：把之前定义的顶点数据复制到缓冲的内存中
	//注意：显卡如何管理给定的数据,有3种：
		//  GL_STATIC_DRAW ：数据不会或几乎不会改变
		//	GL_DYNAMIC_DRAW：数据会被改变很多
		//	GL_STREAM_DRAW ：数据每次绘制时都会改变
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//告诉OpenGL该如何解析顶点数据(应用到逐个顶点属性上)
	glVertexAttribPointer(0,//顶点属性的位置值,shader中location 
		3,                   //顶点属性的大小,vec3
		GL_FLOAT,            //数据类型宏
		GL_FALSE,            //是否所有数据标准化到[0,1](unsigned)或者[-1,-1](signed)
		8 * sizeof(GLfloat),   //stride步长，连续的顶点属性组之间的间隔；若写0，则让openGL决定
		(GLvoid*)NULL);      //位置数据在缓冲中起始位置的偏移量(Offset)
	glEnableVertexAttribArray(0);//顶点属性的位置值index
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);//这2句设置color
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);//这2句设置texture
	glBindVertexArray(0);//解绑VAO,避免在其它地方错误地配置它们。
	glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑VBO 

	//封装好的shader-class,只需要vs和fs的GLSL文件代码路径
	Shader shader("../../Src/Vertex.glsl", "../../Src/Fragment.glsl");
	
	//有关纹理1的设置
	GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	//设置纹理1的环绕方式:GL_REPEAT|GL_MIRRORED_REPEAT|GL_CLAMP_TO_EDGE|GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//设置纹理1过滤(纹理和物体大小不匹配:放大(Magnify)和缩小的时候可以设置纹理过滤选项)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//设置多级渐远纹理1 Mipmap(原纹理的1/4,1/16,1/64...来适配远近纹理)
	//生成mipmap: glGenerateMipmaps,只有缩小纹理过滤才能mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//使用SOIL库(Simple OpenGL Image Library)加载和创建纹理
	int texWidth, texHeight;
	unsigned char* image = SOIL_load_image("../../Resources/container.jpg", &texWidth, &texHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D,//纹理目标(Target)
		0,//Mipmap的级别,0表示基本级别
		GL_RGB,//纹理储存格式
		texWidth,
		texHeight,
		0, //历史遗留问题,必须为0
		GL_RGB,//原图的格式和数据类型,用RGB加载image
		GL_UNSIGNED_BYTE,
		image);//图像数据buffer
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	//有关纹理2的设置
	GLuint texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//设置纹理2的环绕方式:GL_REPEAT|GL_MIRRORED_REPEAT|GL_CLAMP_TO_EDGE|GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//设置纹理2过滤(纹理和物体大小不匹配:放大(Magnify)和缩小的时候可以设置纹理过滤选项)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//SOIL库加载第2张图 
	int texWidth2, texHeight2;
	unsigned char* image2 = SOIL_load_image("../../Resources/awesomeface.png", &texWidth2, &texHeight2, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D,//纹理目标(Target)
		0,//Mipmap的级别,0表示基本级别
		GL_RGB,//纹理储存格式
		texWidth2,
		texHeight2,
		0, //历史遗留问题,必须为0
		GL_RGB,//原图的格式和数据类型,用RGB加载image
		GL_UNSIGNED_BYTE,
		image2);//图像数据buffer
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image2);


	//窗口循环/事件循环
	while (!glfwWindowShouldClose(window))
	{
		//在循环最开始：检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		//然后调用对应的回调函数（可以通过回调方法手动设置）
		glfwPollEvents();

		//渲染主体
		//step1: 设置底色画布
		glClearColor(0.4f, 0.3f, 0.5f, 0.5F);//四个参数RGBA,范围都是[0.0,1.0]
		glClear(GL_COLOR_BUFFER_BIT);
		//step2: 启动Shader-program
		shader.Use();
       
		 // Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), "ourTexture2"), 1);

		glUniform1i(glGetUniformLocation(shader.GetProgram(), "textureAlpha"), gTextureAlpha);

		glBindVertexArray(VAO);//绑定VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//offcial
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);//解绑VAO

		//渲染结束，交换显存相关的buffer，类似刷新显示功能
		glfwSwapBuffers(window);
	}

	// 关闭glfw窗口后清除对应的资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}


//简单功能：按下"ESC"按键退出窗口
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_UP)
		gTextureAlpha = (gTextureAlpha < 100) ? (gTextureAlpha + 1) : gTextureAlpha;
	if (key == GLFW_KEY_DOWN)
		gTextureAlpha = (gTextureAlpha > 0) ? (gTextureAlpha - 1) : gTextureAlpha;
}
