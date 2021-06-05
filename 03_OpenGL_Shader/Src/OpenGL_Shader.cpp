/*使用EBO，绘制了个平行四边形，每个顶点颜色不一*/
#include <iostream>

/*注意：所有中文注释的位置都尽量用这种注释，避免不同系统间的行尾CRLF转换带来bug*/

/*GLEW 必须在glfw前面*/
#define GLEW_STATIC
#include <GLEW/glew.h>

/*GLFW 第三方UI窗口库*/
#include <GLFW/glfw3.h>

/*封装好的shader管理类*/
#include "ShaderClass.hpp"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

/*初始化窗口以及GL函数接口*/
GLFWwindow* InitGLWindowsAndFunction(GLuint width, GLuint height);
/*接收原始数据，并绑定属性到openGL上下文*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO,
    const GLfloat* vertices, GLuint vertexMemSize, const GLuint* indices, GLuint indexMemSize);


//注意这个宏的差异,虽然最终结果是一样的,但是用了EBO是可以肉眼可见的省事儿
#define USE_EBO_DRAW_TRIANGLE  1

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
#if USE_EBO_DRAW_TRIANGLE
    glGenBuffers(1, &EBO);/*创建EBO*/
#endif
    /* 1. 绑定VAO, 再设置顶点属性,到解绑之前,这些上下文属性就都属于这个VAO了,避免了VBO重复执行 */
    glBindVertexArray(VAO);

    /* 2. 把顶点数组复制到缓冲中供OpenGL使用 */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexMemSize, vertices, GL_STATIC_DRAW);

    /* 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexMemSize, indices, GL_STATIC_DRAW);
	
    /* 4. 设置顶点属性指针 */
    /* 顶点属性有positon和color*/
	glVertexAttribPointer(0,//顶点属性的位置值,shader中location 
				3,                   /*顶点属性的大小,vec3*/
				GL_FLOAT,            /*数据类型宏*/
				GL_FALSE,            /*是否所有数据标准化到[0,1](unsigned)或者[-1,-1](signed)*/
				6 * sizeof(GLfloat), /*stride步长，连续的顶点属性组之间的间隔；若写0，则让openGL决定*/
				(GLvoid*)NULL);      /*位置数据在缓冲中起始位置的偏移量(Offset)*/
	glEnableVertexAttribArray(0 /*顶点属性的position-index*/);
	glEnableVertexAttribArray(0);//上面2句设置positon
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1 /*顶点属性的color-index*/);//这2句设置color

    glBindBuffer(GL_ARRAY_BUFFER, 0); /* 解绑VBO,因为glVertexAttribPointer使用ok了*/

    /* 5. 解绑VAO */
    glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);/*解绑EBO。注意：EBO解绑不可以在VAO解绑之前*/
}

/*主程序：运用VAO属性,轻松使用 2 个三角形画出 1 个矩形 */
int main(int argc , char *argv[])
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

    const GLuint WIDTH = 800, HEIGHT = 600;
    GLFWwindow* window = InitGLWindowsAndFunction(WIDTH, HEIGHT);

    Shader shader("../../Src/Vertex.glsl", "../../Src/Fragment.glsl");

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
		// 位置            //颜色
		-0.5f,-0.5f,0.0f,   1.0f,0.0f,0.0f, //  点A:左下
		 0.5f,-0.5f,0.0f,   0.0f,1.0f,0.0f, //  点B:右下
		 0.0f, 0.5f,0.0f,   0.0f,0.0f,1.0f, //  点C:左上 
		 1.0f, 0.5f,0.0f,   0.4f,0.1f,0.5f, //  点D:右上 
    };

    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 2,  // First Triangle
        1, 2, 3   // Second Triangle /*这行可以屏蔽看看,只画 1 个三角形*/
    };
    GLuint VBO, VAO, EBO;
    ProcessBindAttrs(VBO, VAO, EBO, vertices, sizeof(vertices), indices, sizeof(indices));
 
    /* 线框模式画三角形 , 可以放开下面这句注释看效果*/ 
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);/*第二个参数默认是GL_FILL*/
    
    // Game loop
    while (!glfwWindowShouldClose(window)){
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
  
        /*颜色值如果超出范围会自动clamp到[0.0,1.0]*/
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);/*四个参数RGBA,范围都是[0.0,1.0]*/
        glClear(GL_COLOR_BUFFER_BIT);

        /*激活shader*/
        shader.Use();

        /* 更新uniform颜色,如果要启用,对应的要注意 Fragment.glsl 的修改 */
        GLfloat changeValue = (sin(glfwGetTime()) / 2) + 0.5;
        GLint vertexColorLocation = glGetUniformLocation(shader.GetProgram(), "uniformColor");
        glUniform4f(vertexColorLocation, 1.0 - changeValue, changeValue, changeValue, 1.0f);

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
}

