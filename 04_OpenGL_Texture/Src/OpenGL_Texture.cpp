
#include <iostream>

/*ע�⣺��������ע�͵�λ�ö�����������ע�ͣ����ⲻͬϵͳ�����βCRLFת������bug*/

/*GLEW ������glfwǰ��*/
#define GLEW_STATIC
#include <GL/glew.h>

/*GLFW ������UI���ڿ�*/
#include <GLFW/glfw3.h>

/*��װ�õ�shader������*/
#include "ShaderClass.hpp"

/* ����SOIL */
#include "SOIL/SOIL.h"

// Function prototypes
/*���ڰ���������Ӧ�ص�����*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

/*��ʼ�������Լ�GL�����ӿ�*/
GLFWwindow* InitGLWindowsAndFunction(GLuint width, GLuint height);

/*����ԭʼ���ݣ��������Ե�openGL������*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO,
    const GLfloat* vertices, GLuint vertexMemSize, const GLuint* indices, GLuint indexMemSize);

/* ʹ��һ��ͼƬ����һ��texture����, ���������� */
GLuint CreateTextureWithImage(const char* texImagePath);

/*͸���ȵ��� */
GLuint gTextureAlpha = 50;

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

/*����ԭʼ���ݣ��������Ե�openGL������*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO, 
    const GLfloat * vertices,GLuint vertexMemSize, 
    const GLuint * indices,GLuint indexMemSize)
{
    glGenVertexArrays(1, &VAO);/*����VAO*/
    glGenBuffers(1, &VBO);/*����VAO*/
    glGenBuffers(1, &EBO);/*����EBO*/

    /* 1. ��VAO, �����ö�������,�����֮ǰ,��Щ���������ԾͶ��������VAO��,������VBO�ظ�ִ�� */
    glBindVertexArray(VAO);

    /* 2. �Ѷ������鸴�Ƶ������й�OpenGLʹ�� */
	/* #ע�⣺�Կ���ι������������,��3�֣�
	 *@	GL_STATIC_DRAW �����ݲ���򼸺�����ı�
	 *@	GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶ�
	 *@	GL_STREAM_DRAW ������ÿ�λ���ʱ����ı�
	 */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexMemSize, vertices, GL_STATIC_DRAW);

    /* 3. �������ǵ��������鵽һ�����������У���OpenGLʹ��*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexMemSize, indices, GL_STATIC_DRAW);
	
    /* 4. ���ö�������ָ�� */
    /* ����������positon��color�Լ�texture*/
	glVertexAttribPointer(0,//�������Ե�λ��ֵ,shader��location 
				3,                   /*�������ԵĴ�С,vec3*/
				GL_FLOAT,            /*�������ͺ�*/
				GL_FALSE,            /*�Ƿ��������ݱ�׼����[0,1](unsigned)����[-1,-1](signed)*/
				8 * sizeof(GLfloat), /*stride�����������Ķ���������֮��ļ������д0������openGL����*/
				(GLvoid*)NULL);      /*λ�������ڻ�������ʼλ�õ�ƫ����(Offset)*/
	glEnableVertexAttribArray(0 /*position-index*/);/*����2������positon */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1 /*color-index*/);//��2������color
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2 /*texture-index*/);//��2������texture

    glBindBuffer(GL_ARRAY_BUFFER, 0); /* ���VBO,��ΪglVertexAttribPointerʹ��ok��*/

    /* 5. ���VAO */
    glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);/*���EBO��ע�⣺EBO��󲻿�����VAO���֮ǰ*/
}

/* ʹ��һ��ͼƬ����һ��texture����, ���������� */
GLuint CreateTextureWithImage(const char* texImagePath)
{
    /*�й�����1������*/
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    /*��������1�Ļ��Ʒ�ʽ:GL_REPEAT|GL_MIRRORED_REPEAT|GL_CLAMP_TO_EDGE|GL_CLAMP_TO_BORDER*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    /*��������1����(����������С��ƥ��:�Ŵ�(Magnify)����С��ʱ����������������ѡ��)*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /*���ö༶��Զ����1 Mipmap(ԭ�����1/4,1/16,1/64...������Զ������)*/
    /*����mipmap: glGenerateMipmaps,ֻ����С������˲���mipmap*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    /*ʹ��SOIL��(Simple OpenGL Image Library)���غʹ�������*/
    int texWidth, texHeight;
    unsigned char* image = SOIL_load_image(texImagePath, &texWidth, &texHeight, 0, SOIL_LOAD_RGB);
    if (!image) {
        std::cout << "Can't load this picture:" << texImagePath << " ,please check!\n";
        return GL_INVALID_VALUE;
    }
    glTexImage2D(GL_TEXTURE_2D,/* ����Ŀ��(Target)*/
        0,        /*Mipmap�ļ���, 0��ʾ�������� */
        GL_RGB,   /*�������ʽ */
        texWidth,
        texHeight,
        0,        /*��ʷ��������,����Ϊ0 */
        GL_RGB,   /*ԭͼ�ĸ�ʽ����������,��RGB����image */
        GL_UNSIGNED_BYTE,
        image);   /*ͼ������buffer */
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    
    return texture;
}

/*�����򣺽�2��ͼ��Ⱦ��һ�����ο���,���ҿ����÷��������͸���� */
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
		//     ---- λ�� ----       ---- ��ɫ ----     - �������� -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
    };

    GLuint indices[] = {  /* vertices�Ķ������� 0,1,2,3 */
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
  
        /*��ɫֵ���������Χ���Զ�clamp��[0.0,1.0]*/
        glClearColor(0.2f, 0.3f, 0.3f, 0.5f);/*�ĸ�����RGBA,��Χ����[0.0,1.0]*/
        glClear(GL_COLOR_BUFFER_BIT);

        /*����shader*/
        shader.Use();

        /*uniform ֵ��������Ԫ���� */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), "ourTexture2"), 1);

		glUniform1i(glGetUniformLocation(shader.GetProgram(), "textureAlpha"), gTextureAlpha);

        glBindVertexArray(VAO);/*��VAO,Ӧ��ǰ�����õ�����*/
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);/*���ö���������ͼ*/       
        glBindVertexArray(0);/*���VAO */

        glfwSwapBuffers(window);// Swap the screen buffers
    }
    /*ע���ڴ������*/
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

