
#include <iostream>

/*ע�⣺��������ע�͵�λ�ö�����������ע�ͣ����ⲻͬϵͳ�����βCRLFת������bug*/

/*GLEW ������glfwǰ��*/
#define GLEW_STATIC
#include <GLEW/glew.h>

/*GLFW ������UI���ڿ�*/
#include <GLFW/glfw3.h>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

/*��ʼ�������Լ�GL�����ӿ�*/
GLFWwindow* InitGLWindowsAndFunction(GLuint width, GLuint height);
/*����shader���봴��program*/
GLuint CreateShaderProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);
/*����ԭʼ���ݣ��������Ե�openGL������*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO,
    const GLfloat* vertices, GLuint vertexMemSize, const GLuint* indices, GLuint indexMemSize);

// Shaders: vertex-shader and  fragment-shader
const GLchar* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";
/*shader����GPUִ��,CPU���������ַ���,2��ע�Ͷ��У��������Ƿŵ��ı��ļ�ȥ����*/
const GLchar* fragmentShaderSource = "#version 330 core\n \
	out vec4 color;\n \
	void main()\n \
	{\n \
	color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n \
	}\n\0";
/*��2��fragment-shader,��һ������ɫ*/
const GLchar* fragmentShaderSource2nd = "#version 330 core\n \
	out vec4 color;\n \
	void main()\n \
	{\n \
	color = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n \
	}\n\0";

//ע�������Ĳ���,��Ȼ���ս����һ����,��������EBO�ǿ������ۿɼ���ʡ�¶�
#define USE_EBO_DRAW_TRIANGLE  1

/* ��ʼ�����ڶ�������Ϊ�̻���������ȡ�ɺ���*/
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
    if (glewInit() != GLEW_OK) {   /*ע�⣺�������initִ��֮�����ִ������gl����*/
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

/*����ԭʼ���ݣ��������Ե�openGL������*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO, 
    const GLfloat * vertices,GLuint vertexMemSize, 
    const GLuint * indices,GLuint indexMemSize)
{
    glGenVertexArrays(1, &VAO);/*����VAO*/
    glGenBuffers(1, &VBO);/*����VAO*/
#if USE_EBO_DRAW_TRIANGLE
    glGenBuffers(1, &EBO);/*����EBO*/
#endif
    /* 1. ��VAO, �����ö�������,�����֮ǰ,��Щ���������ԾͶ��������VAO��,������VBO�ظ�ִ�� */
    glBindVertexArray(VAO);

    /* 2. �Ѷ������鸴�Ƶ������й�OpenGLʹ�� */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexMemSize, vertices, GL_STATIC_DRAW);

    /* 3. �������ǵ��������鵽һ�����������У���OpenGLʹ��*/
#if USE_EBO_DRAW_TRIANGLE
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexMemSize, indices, GL_STATIC_DRAW);
#endif
    /* 4. ���ö�������ָ�� */
    glVertexAttribPointer(0, 3/*������������ά��*/, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // ���VBO,��ΪglVertexAttribPointerʹ��ok��

    /* 5. ���VAO */
    glBindVertexArray(0);
}

/*����������VAO����,����ʹ�� 2 �������λ��� 1 ������ */
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
        1, 2, 3   // Second Triangle /*���п������ο���,ֻ�� 1 ��������*/
    };
    GLuint indices2nd[] = {0,1,4};/*ʹ�õ�2�������ٻ�һ��������,ʹ�ú͵�һ�鲻ͬ��ɫ */
    GLuint VBO[2], VAO[2], EBO[2];
    ProcessBindAttrs(VBO[0], VAO[0], EBO[0], vertices, sizeof(vertices), indices, sizeof(indices));
    ProcessBindAttrs(VBO[1], VAO[1], EBO[1], vertices, sizeof(vertices), indices2nd, sizeof(indices2nd));
    
    /* �߿�ģʽ�������� , ���Էſ��������ע�Ϳ�Ч��*/ 
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
        glBindVertexArray(VAO[0]);/*��VAO[0],Ӧ��ǰ�����õ�����*/
#if !USE_EBO_DRAW_TRIANGLE
        glDrawArrays(GL_TRIANGLES, 0/*first*/, 6/*count*/);/*���ö��������ͼ,1��������Ҫ�о�6������*/
#else
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);/*���ö���������ͼ��1��������Ҫ�о�4������ ��ָ������*/
#endif         
        glBindVertexArray(0);/*���VAO */

        //Draw sencond triangle-group
        glUseProgram(shaderProgram2nd);
        glBindVertexArray(VAO[1]);/*��VAO[1],Ӧ��ǰ�����õ�����*/
#if !USE_EBO_DRAW_TRIANGLE
        glDrawArrays(GL_TRIANGLES, 6, 3);
#else
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
#endif
        glBindVertexArray(0);/*���VAO */

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

