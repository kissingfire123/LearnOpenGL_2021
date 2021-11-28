
#include <iostream>

/*注意：所有中文注释的位置都尽量用这种注释，避免不同系统间的行尾CRLF转换带来bug*/

/*GLEW 必须在glfw前面*/
#define GLEW_STATIC
#include <GL/glew.h>

/*GLFW 第三方UI窗口库*/
#include <GLFW/glfw3.h>

/*封装好的shader管理类*/
#include "ShaderClass.hpp"

/* 引入SOIL */
#include "SOIL/SOIL.h"

// Function prototypes
/*窗口按键操作响应回调函数*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

/*初始化窗口以及GL函数接口*/
GLFWwindow* InitGLWindowsAndFunction(GLuint width, GLuint height);

/*接收原始数据，并绑定属性到openGL上下文*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO,
    const GLfloat* vertices, GLuint vertexMemSize, const GLuint* indices, GLuint indexMemSize);

/* 使用一张图片创建一个texture纹理, 并设置属性 */
GLuint CreateTextureWithImage(const char* texImagePath);

/*透明度调节 */
GLuint gTextureAlpha = 50;

/* 初始化窗口动作，较为固化独立，抽取成函数*/
GLFWwindow*  InitGLWindowsAndFunction(GLuint width , GLuint height)
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW,opengl-3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK) {   /*注意：这个函数init执行之后才能执行其他gl函数*/
        std::cout << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }

    // Define the viewport dimensions
    int glwidth, glheight;
    glfwGetFramebufferSize(window, &glwidth, &glheight);
    glViewport(0, 0, glwidth, glheight);
    return window;
}

/*接收原始数据，并绑定属性到openGL上下文*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO, 
    const GLfloat * vertices,GLuint vertexMemSize, 
    const GLuint * indices,GLuint indexMemSize)
{
    glGenVertexArrays(1, &VAO);/*创建VAO*/
    glGenBuffers(1, &VBO);/*创建VAO*/
    glGenBuffers(1, &EBO);/*创建EBO*/

    /* 1. 绑定VAO, 再设置顶点属性,到解绑之前,这些上下文属性就都属于这个VAO了,避免了VBO重复执行 */
    glBindVertexArray(VAO);

    /* 2. 把顶点数组复制到缓冲中供OpenGL使用 */
	/* #注意：显卡如何管理给定的数据,有3种：
	 *@	GL_STATIC_DRAW ：数据不会或几乎不会改变
	 *@	GL_DYNAMIC_DRAW：数据会被改变很多
	 *@	GL_STREAM_DRAW ：数据每次绘制时都会改变
	 */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexMemSize, vertices, GL_STATIC_DRAW);

    /* 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexMemSize, indices, GL_STATIC_DRAW);
	
    /* 4. 设置顶点属性指针 */
    /* 顶点属性有positon和color以及texture*/
	glVertexAttribPointer(0,//顶点属性的位置值,shader中location 
				3,                   /*顶点属性的大小,vec3*/
				GL_FLOAT,            /*数据类型宏*/
				GL_FALSE,            /*是否所有数据标准化到[0,1](unsigned)或者[-1,-1](signed)*/
				8 * sizeof(GLfloat), /*stride步长，连续的顶点属性组之间的间隔；若写0，则让openGL决定*/
				(GLvoid*)NULL);      /*位置数据在缓冲中起始位置的偏移量(Offset)*/
	glEnableVertexAttribArray(0 /*position-index*/);/*上面2句设置positon */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1 /*color-index*/);//这2句设置color
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2 /*texture-index*/);//这2句设置texture

    glBindBuffer(GL_ARRAY_BUFFER, 0); /* 解绑VBO,因为glVertexAttribPointer使用ok了*/

    /* 5. 解绑VAO */
    glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);/*解绑EBO。注意：EBO解绑不可以在VAO解绑之前*/
}

/* 使用一张图片创建一个texture纹理, 并设置属性 */
GLuint CreateTextureWithImage(const char* texImagePath)
{
    /*有关纹理1的设置*/
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    /*设置纹理1的环绕方式:GL_REPEAT|GL_MIRRORED_REPEAT|GL_CLAMP_TO_EDGE|GL_CLAMP_TO_BORDER*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    /*设置纹理1过滤(纹理和物体大小不匹配:放大(Magnify)和缩小的时候可以设置纹理过滤选项)*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /*设置多级渐远纹理1 Mipmap(原纹理的1/4,1/16,1/64...来适配远近纹理)*/
    /*生成mipmap: glGenerateMipmaps,只有缩小纹理过滤才能mipmap*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    /*使用SOIL库(Simple OpenGL Image Library)加载和创建纹理*/
    int texWidth, texHeight;
    unsigned char* image = SOIL_load_image(texImagePath, &texWidth, &texHeight, 0, SOIL_LOAD_RGB);
    if (!image) {
        std::cout << "Can't load this picture:" << texImagePath << " ,please check!\n";
        return GL_INVALID_VALUE;
    }
    glTexImage2D(GL_TEXTURE_2D,/* 纹理目标(Target)*/
        0,        /*Mipmap的级别, 0表示基本级别 */
        GL_RGB,   /*纹理储存格式 */
        texWidth,
        texHeight,
        0,        /*历史遗留问题,必须为0 */
        GL_RGB,   /*原图的格式和数据类型,用RGB加载image */
        GL_UNSIGNED_BYTE,
        image);   /*图像数据buffer */
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    
    return texture;
}

/*主程序：将2张图渲染到一个矩形框中,并且可以用方向键调控透明度 */
int main(int argc , char *argv[])
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

    const GLuint WIDTH = 800, HEIGHT = 600;
    GLFWwindow* window = InitGLWindowsAndFunction(WIDTH, HEIGHT);
#if WIN32
    Shader shader("../../Src/Vertex.glsl", "../../Src/Fragment.glsl");
#else
    Shader shader("../../../Src/Vertex.glsl", "../../../Src/Fragment.glsl");
#endif
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };

    GLuint indices[] = {  /* vertices的顶点索引 0,1,2,3 */
        0, 1, 3,  // First Triangle
        1, 2, 3  
    };
    GLuint VBO, VAO, EBO;
    ProcessBindAttrs(VBO, VAO, EBO, vertices, sizeof(vertices), indices, sizeof(indices));

    GLuint texture1 = CreateTextureWithImage("../../Resources/container.jpg");
    GLuint texture2 = CreateTextureWithImage("../../Resources/husky.jpg");
    if (texture1 == GL_INVALID_VALUE || texture2 == GL_INVALID_VALUE) {
        std::cout << "Create texture failed, please check!\n";
    }
    
    // Game loop
    while (!glfwWindowShouldClose(window)){
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
  
        /*颜色值如果超出范围会自动clamp到[0.0,1.0]*/
        glClearColor(0.2f, 0.3f, 0.3f, 0.5f);/*四个参数RGBA,范围都是[0.0,1.0]*/
        glClear(GL_COLOR_BUFFER_BIT);

        /*激活shader*/
        shader.Use();

        /*uniform 值设置纹理单元属性 */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), "ourTexture2"), 1);

		glUniform1i(glGetUniformLocation(shader.GetProgram(), "textureAlpha"), gTextureAlpha);

        glBindVertexArray(VAO);/*绑定VAO,应用前面设置的属性*/
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);/*利用顶点索引作图*/       
        glBindVertexArray(0);/*解绑VAO */

        glfwSwapBuffers(window);// Swap the screen buffers
    }
    /*注意内存的销毁*/
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_UP)
		gTextureAlpha = (gTextureAlpha < 100) ? (gTextureAlpha + 1) : gTextureAlpha;
	if (key == GLFW_KEY_DOWN)
		gTextureAlpha = (gTextureAlpha > 0) ? (gTextureAlpha - 1) : gTextureAlpha;
}

