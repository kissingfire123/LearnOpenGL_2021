
#include <iostream>

/*注意：所有中文注释的位置都尽量用这种注释，避免不同系统间的行尾CRLF转换带来bug*/

/*GLEW 必须在glfw前面*/
#define GLEW_STATIC
#include <GLEW/glew.h>

/*GLFW 第三方UI窗口库*/
#include <GLFW/glfw3.h>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

/*初始化窗口以及GL函数接口*/
GLFWwindow* InitGLWindowsAndFunction(GLuint width, GLuint height);
/*根据shader代码创建program*/
GLuint CreateShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
/*接收原始数据，并绑定属性到openGL上下文*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO,
    const GLfloat* vertices, GLuint vertexMemSize, const GLuint* indices, GLuint indexMemSize);

// Shaders: vertex-shader and  fragment-shader
const GLchar* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";
/*shader交给GPU执行,CPU看来就是字符串,2种注释都行，后续考虑放到文本文件去读入*/
const GLchar* fragmentShaderSource = "#version 330 core\n \
	out vec4 color;\n \
	void main()\n \
	{\n \
	color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n \
	}\n\0";
/*第2个fragment-shader,不一样的着色*/
const GLchar* fragmentShaderSource2nd = "#version 330 core\n \
	out vec4 color;\n \
	void main()\n \
	{\n \
	color = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n \
	}\n\0";

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

GLuint CreateShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
{
    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return GL_INVALID_VALUE;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return GL_INVALID_VALUE;
    }
    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return GL_INVALID_VALUE;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
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
#if USE_EBO_DRAW_TRIANGLE
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexMemSize, indices, GL_STATIC_DRAW);
#endif
    /* 4. 设置顶点属性指针 */
    glVertexAttribPointer(0, 3/*单个顶点坐标维度*/, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO,因为glVertexAttribPointer使用ok了

    /* 5. 解绑VAO */
    glBindVertexArray(0);
}

/*主程序：运用VAO属性,轻松使用 2 个三角形画出 1 个矩形 */
int main(int argc , char *argv[])
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

    const GLuint WIDTH = 800, HEIGHT = 600;
    GLFWwindow* window = InitGLWindowsAndFunction(WIDTH, HEIGHT);
 
    GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    GLuint shaderProgram2nd = CreateShaderProgram( vertexShaderSource, fragmentShaderSource2nd);

    // Set up vertex data (and buffer(s)) and attribute pointers
#if !USE_EBO_DRAW_TRIANGLE
    GLfloat vertices[] = {
      // First triangle
       0.5f,  0.5f, 0.0f, // Top Right
       0.5f, -0.5f, 0.0f, // Bottom Right
      -0.5f,  0.5f, 0.0f, // Top Left 
      // Second triangle
       0.5f, -0.5f, 0.0f, // Bottom Right
      -0.5f, -0.5f, 0.0f, // Bottom Left
      -0.5f,  0.5f, 0.0f, // Top Left
      //Third triangle
       0.5f,  0.5f, 0.0f,  // Top Right
       0.5f, -0.5f, 0.0f,  // Bottom Right
       1.0f,  0.0f, 0.0f  // the most right point
    };
#else
    GLfloat vertices[] = {
         0.5f,  0.5f, 0.0f,  // Top Right
         0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f,   // Top Left 
         1.0f,  0.0f, 0.0f
    };
#endif
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,  // First Triangle
        1, 2, 3   // Second Triangle /*这行可以屏蔽看看,只画 1 个三角形*/
    };
    GLuint indices2nd[] = {0,1,4};/*使用第2组索引再画一组三角形,使用和第一组不同着色 */
    GLuint VBO[2], VAO[2], EBO[2];
    ProcessBindAttrs(VBO[0], VAO[0], EBO[0], vertices, sizeof(vertices), indices, sizeof(indices));
    ProcessBindAttrs(VBO[1], VAO[1], EBO[1], vertices, sizeof(vertices), indices2nd, sizeof(indices2nd));
    
    /* 线框模式画三角形 , 可以放开下面这句注释看效果*/ 
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Game loop
    while (!glfwWindowShouldClose(window)){
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render   
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// Clear the colorbuffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw our first triangle-group
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO[0]);/*绑定VAO[0],应用前面设置的属性*/
#if !USE_EBO_DRAW_TRIANGLE
        glDrawArrays(GL_TRIANGLES, 0/*first*/, 6/*count*/);/*利用多个顶点作图,1个矩形需要列举6个顶点*/
#else
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);/*利用顶点索引作图，1个矩形需要列举4个顶点 并指定索引*/
#endif         
        glBindVertexArray(0);/*解绑VAO */

        //Draw sencond triangle-group
        glUseProgram(shaderProgram2nd);
        glBindVertexArray(VAO[1]);/*绑定VAO[1],应用前面设置的属性*/
#if !USE_EBO_DRAW_TRIANGLE
        glDrawArrays(GL_TRIANGLES, 6, 3);
#else
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
#endif
        glBindVertexArray(0);/*解绑VAO */

        glfwSwapBuffers(window);// Swap the screen buffers
    }
    // Properly de-allocate all resources once they've outlived their purpose
    for (int i = 0; i < 2; ++i) { 
        glDeleteVertexArrays(1, &VAO[i]);
        glDeleteBuffers(1, &VBO[i]);
        glDeleteBuffers(1, &EBO[i]);
    }
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

