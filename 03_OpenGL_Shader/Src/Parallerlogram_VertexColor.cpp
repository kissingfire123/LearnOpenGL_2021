//使用EBO，绘制了个平行四边形，每个顶点颜色不一
#if 0
#include <iostream>

//引入GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//引入GLFW
#include <GLFW/glfw3.h>

//回调函数
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

//窗口维度
const GLuint WIDTH = 800, HEIGHT = 600;

//着色器 shader:着色器语言GLSL(OpenGL Shading Language)
//顶点着色器 vertexShader
//复杂shader:每个顶点带有不同的color
const char* vertexShaderSource = "#version 330 core \n"
"layout (location =0) in vec3 position; \n"
"layout (location =1) in vec3 color; \n"
"out vec3 ourColor;//向fragment-Shader输出一个color \n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position,1.0);\n"
"	ourColor = color;\n"
"}\n\0";
#if 0 //普通shader，输出vertexColor给fragment-Shader
const char* vertexShaderSource = "#version 330 core \n"
"layout (location = 0) in vec3 position; \n "
"out vec4 vertexColor ;\n"
"void main() \n"
"{ \n"
"     gl_Position = vec4(position.x,position.y,position.z,1.0); \n"
"     vertexColor = vec4(1.0f,0.0f,0.0f,1.0f);\n"
"}  \n\0";//注意：末尾不要忘记结束符"\0"
#endif


//片元着色器 注：GLSL语言的字符串用转行提示符"\"这样给出也可以
const char* fragmentShaderSource = "#version 330 core\n \
 in vec3 ourColor; \n\
 out vec4 color; \n\
 void main() \n\
 {\n\
	color = vec4(ourColor,1.0f) ;\n\
 }\n\0";
#if 0 //使用了uniform的shader
const char* fragmentShaderSource = "#version 330 core\n \
 out vec4 color; \n\
 uniform vec4 ourColor; \n\
 void main() \n\
 {\n\
	color = ourColor ;\n\
 }\n\0";
#endif
#if 0 //普通shader，接收vertex的out
const char* fragmentShaderSource = "#version 330 core\n \
 in  vec4 vertexColor ;\n\
 out vec4 color; \n\
 void main() \n\
 {\n\
 	color = vertexColor; \n\
 }\n\0";//注意：末尾不要忘记结束符"\0"
#endif

int main(int argc, char *argv[]) 
{
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


	//==================>> 创建顺序和解绑顺序一般都采用：VAO-->VBO-->EBO  
	//顶点数组对象(Vertex Array Object, VAO)
	//渲染前指定OpenGL该如何解释顶点数据
	GLuint VAO;
	glGenVertexArrays(1/*size_n*/, &VAO);
	glBindVertexArray(VAO);// 绑定VAO

	//定义平行四边形顶点：(2个三角形组成,使用EBO可以复用2个顶点!)
	GLfloat vertices[] = {
		// 位置            //颜色
		-0.5f,-0.5f,0.0f,   1.0f,0.0f,0.0f, //  点A:左下
		 0.5f,-0.5f,0.0f,   0.0f,1.0f,0.0f, //  点B:右下
		 0.0f, 0.5f,0.0f,   0.0f,0.0f,1.0f, //  点C:左上 
		 1.0f, 0.5f,0.0f,   0.4f,0.1f,0.5f, //  点D:右上 
	};
	//顶点缓冲对象(Vertex Buffer Objects, VBO),一次性的发送一大批数据到显卡
	GLuint VBO;
	glGenBuffers(1, &VBO); //生成一个VBO对象
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER
	//glBufferData ：把之前定义的顶点数据复制到缓冲的内存中
	//注意：显卡如何管理给定的数据,有3种：
		//  GL_STATIC_DRAW ：数据不会或几乎不会改变
		//	GL_DYNAMIC_DRAW：数据会被改变很多
		//	GL_STREAM_DRAW ：数据每次绘制时都会改变
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//三角形的顶点索引,为EBO准备
	GLuint indices[] = {
		0,2,3,//第1个三角形 
		0,1,3 //第2个三角形
	};
	//索引缓冲对象(Element Buffer Object，EBO，也叫Index Buffer Object，IBO),在VBO之后,调用和VBO雷同
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//告诉OpenGL该如何解析顶点数据(应用到逐个顶点属性上)
#if 0 //顶点属性只有位置的情况
	glVertexAttribPointer(0,//顶点属性的位置值,shader中location 
		3,                   //顶点属性的大小,vec3
		GL_FLOAT,            //数据类型宏
		GL_FALSE,            //是否所有数据标准化到[0,1](unsigned)或者[-1,-1](signed)
		3 * sizeof(GLfloat),   //stride步长，连续的顶点属性组之间的间隔；若写0，则让openGL决定
		(GLvoid*)NULL);      //位置数据在缓冲中起始位置的偏移量(Offset)
	glEnableVertexAttribArray(0);//顶点属性的位置值index
#endif
#if 1// 顶点属性有positon和color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)NULL);
	glEnableVertexAttribArray(0);//这2句设置positon
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);//这2句设置color
#endif
	glBindVertexArray(0);//解绑VAO,避免在其它地方错误地配置它们。
	glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑VBO 
	//VAO会储存glBindBuffer的函数调用,包括储存解绑调用,不要在解绑VAO之前解绑EBO,否则它就没有这个EBO配置了,导致OpenGL不作图
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//解绑EBO。注意：EBO解绑不可以在VAO解绑之前



	//创建一个vertex-Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//把这个着色器源码附加到着色器对象上
	glShaderSource(vertexShader, 1/*源码字符串数量*/, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);//编译这个shader
	//检查shader的编译是否成功，并捕获报错信息
	GLint success;
	GLchar infoLog[512] = { 0 };
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL/*指定int存log字符串长度*/, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//创建一个fragment shader (流程和Vertex-shader类似)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL/*指定int存log字符串长度*/, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//链接Shaders:两个着色器对象链接到一个用来渲染的着色器程序(Shader Program)中
	GLuint shaderProgram = glCreateProgram();
	//把之前编译的着色器附加到程序对象上
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//同样地，是否链接成功，可以查询到状态以及infoLog
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//shader链接到程序对象以后,不再需要它们,记得删除
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//窗口循环/事件循环
	while (!glfwWindowShouldClose(window))
	{
		//在循环最开始：检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		//然后调用对应的回调函数（可以通过回调方法手动设置）
		glfwPollEvents();

		//渲染主体
		//step1: 设置底色画布
		glClearColor(0.4f, 0.1f, 0.5f, 0.5F);//四个参数RGBA,范围都是[0.0,1.0],(0.4,0.1,0.5)近似Ubuntu的紫色
		glClear(GL_COLOR_BUFFER_BIT);
		//step2: 启动Shader-program
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);//绑定VAO
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		
#if 0
		//设置shader程序的uniform变量
		GLfloat timeValue = glfwGetTime();//获取运行的秒数
		GLfloat redValue = (sin(timeValue) / 2) + 0.5;//得到一个[0.0,1.0]范围的浮点数
		GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");//Shader程序中Unfiorm变量名
#endif
		glUseProgram(shaderProgram);
#if 0
		glUniform4f(vertexColorLocation, redValue, redValue, 0.0f, 1.0f);
#endif
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)NULL);//最后一个参数:指定EBO中的偏移量（或者传递一个索引数组）
		glBindVertexArray(0);//解绑VAO

		//渲染结束，交换显存相关的buffer，类似刷新显示功能
		glfwSwapBuffers(window);
	}
	//释放VAO,VBO,EBO申请的资源
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// 关闭glfw窗口后清除对应的资源
	glfwTerminate();
	return 0;
}


//简单功能：按下"ESC"按键退出窗口
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

#endif