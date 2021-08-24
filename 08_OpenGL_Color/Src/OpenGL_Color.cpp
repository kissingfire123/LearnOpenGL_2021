/* OpenGL_Camra.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。 */
/* 基于openGL_CoordinateSystem的基础加入摄像机*/
#include <iostream>
#include <iomanip>

/*引入GLEW*/
#define GLEW_STATIC
#include <GLEW/glew.h>

/*引入GLFW*/
#include <GLFW/glfw3.h>

#include "ShaderClass.hpp"
/* camera 控制类*/
#include "CameraController.hpp"

/*引入SOIL*/
#include "SOIL/SOIL.h"

/*引入 glm*/
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/* 回调函数*/
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

/* 控制相机运动: 并且，同时可以多个方向键生效*/
void do_movement();

/*鼠标控制yaw偏航角和pitch俯仰角*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/*通过鼠标的滚轮来缩放物体*/
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/*初始化窗口以及GL函数接口*/
GLFWwindow* InitGLWindowsAndFunction(GLuint width, GLuint height);

/*接收原始数据，并绑定属性到openGL上下文*/
void ProcessBindAttrs(GLuint& VBO, GLuint& VAO, GLuint& EBO,
    const GLfloat* vertices, GLuint vertexMemSize, const GLuint* indices, GLuint indexMemSize);

/* 使用一张图片创建一个texture纹理, 并设置属性 */
GLuint CreateTextureWithImage(const char* texImagePath);



bool g_keys[1024]{ false };
GLfloat g_deltaTime = 0.0f;   /*当前帧和上一帧的时间差,消除硬件差距导致体验差距 */
// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 4.0f));

/* 初始化窗口动作，较为固化独立，抽取成函数*/
GLFWwindow*  InitGLWindowsAndFunction(GLuint width, GLuint height)
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
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    glfwSetCursorPosCallback(window, mouse_callback);/*鼠标移动控制yaw和pitch*/
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);/*隐藏鼠标并限制在画框内，靠ESC退出*/
    glfwSetScrollCallback(window, scroll_callback);/*鼠标滚轮改变fov，实现缩放*/

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
    const GLfloat * vertices, GLuint vertexMemSize,
    const GLuint * indices, GLuint indexMemSize,
    const GLuint stride)
{
    glGenVertexArrays(1, &VAO);/*创建VAO*/
    glGenBuffers(1, &VBO);/*创建VAO*/
    if (EBO != GL_INVALID_VALUE) {
        glGenBuffers(1, &EBO);/*创建EBO*/
    }
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
    if (EBO != GL_INVALID_VALUE) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexMemSize, indices, GL_STATIC_DRAW);
    }

    /* 4. 设置顶点属性指针 */
    /* 顶点属性有positon和color以及texture*/
    glVertexAttribPointer(0,//顶点属性的位置值,shader中location 
        3,                   /*顶点属性的大小,vec3*/
        GL_FLOAT,            /*数据类型宏*/
        GL_FALSE,            /*是否所有数据标准化到[0,1](unsigned)或者[-1,-1](signed)*/
        stride * sizeof(GLfloat), /*stride步长，连续的顶点属性组之间的间隔；若写0，则让openGL决定*/
        (GLvoid*)NULL);      /*位置数据在缓冲中起始位置的偏移量(Offset)*/
    glEnableVertexAttribArray(0 /*position-index*/);/*上面2句设置positon */

    glBindBuffer(GL_ARRAY_BUFFER, 0); /* 解绑VBO,因为glVertexAttribPointer使用ok了*/

    /* 5. 解绑VAO */
    glBindVertexArray(0);
    if (EBO != GL_INVALID_VALUE) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);/*解绑EBO。注意：EBO解绑不可以在VAO解绑之前*/
    }
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

/*主程序：坐标系统变换
 * 1. 通过宏控制，画1个或者是同时10个旋转的立方体
 * 2. 在旋转过程中，上下方向仍然可以控制表情包透明度
 * 3. fragment-shader中有3种texture效果可以选择修改
 */
int main(int argc, char *argv[])
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

    const GLuint WIDTH = 800, HEIGHT = 600;
    GLFWwindow* window = InitGLWindowsAndFunction(WIDTH, HEIGHT);

    Shader containerShader("../../Src/Vertex.glsl", "../../Src/Fragment.glsl");
    Shader lightShader("../../Src/VertexLight.glsl", "../../Src/FragmentLight.glsl");
    /*定义三角形顶点,每个面6个顶点(2个重复点),共36个顶点描述一个立方体*/
    float vertices[] = {
        /*  ---- 位置 ----       */
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

    };

    // Light attributes
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    GLuint VBO, containerVAO, lightVAO, EBO = GL_INVALID_VALUE;
    ProcessBindAttrs(VBO, containerVAO, EBO, vertices, sizeof(vertices), nullptr, 0, 3);
    ProcessBindAttrs(VBO, lightVAO, EBO, vertices, sizeof(vertices), nullptr, 0, 3);

    /* 开启深度测试 */
    glEnable(GL_DEPTH_TEST);


    //窗口循环/事件循环
    while (!glfwWindowShouldClose(window)) {
        /* 在循环最开始：检查有没有触发什么事件（比如键盘输入、鼠标移动等）
         * 然后调用对应的回调函数（可以通过回调方法手动设置）*/
        glfwPollEvents();
        do_movement();
        /*目的：消除硬件差异，均衡化移动速度*/
        GLfloat currentFrame = glfwGetTime();
        static GLfloat lastFrame = 0.0f;   /*上一帧的时间*/
        g_deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //渲染主体
        /* step1: 设置底色画布*/
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//四个参数RGBA,范围都是[0.0,1.0]
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /*开启深度测试后，需clear深度buffer*/

        //step2: 启动Shader-program
        containerShader.Use();
        GLint objectColorLoc = glGetUniformLocation(containerShader.GetProgram(), "objectColor");
        GLint lightColorLoc = glGetUniformLocation(containerShader.GetProgram(), "lightColor");
        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f); // 物体rgb颜色
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);  // 光照rgb颜色

        /*step3:  传递MVP矩阵给shader*/
        /*MVP 变换: V-clip = M-projection * M-view * M-model * V-local */
        glm::mat4  view(1.0), projection(1.0); /*初始化为单位矩阵和radians 都非常重要*/
        view = camera.GetViewMatrix();
        projection = glm::perspective(camera.GetZoom(), static_cast<GLfloat>(WIDTH) / HEIGHT, 0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(containerShader.GetProgram(), "model");
        GLint viewLoc = glGetUniformLocation(containerShader.GetProgram(), "view");
        GLint projLoc = glGetUniformLocation(containerShader.GetProgram(), "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        /*step4: 渲染绘制*/
        glBindVertexArray(containerVAO);/* 绑定VAO*/
        glm::mat4 model(1.0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);//解绑VAO
// ====================== 以上为container，下述为light
        lightShader.Use();
        modelLoc = glGetUniformLocation(lightShader.GetProgram(), "model");
        viewLoc = glGetUniformLocation(lightShader.GetProgram(), "view");
        projLoc = glGetUniformLocation(lightShader.GetProgram(), "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        /*渲染结束，交换显存相关的buffer，类似刷新显示功能*/
        glfwSwapBuffers(window);
    }

    /* 关闭glfw窗口后清除对应的资源*/
    glDeleteVertexArrays(1, &containerVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}


/*简单功能：按下"ESC"按键退出窗口,且方向键可以控制texture2的透明度*/
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);



    /*键盘控制,enable同一时间多键位生效*/
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            g_keys[key] = true;
        else if (action == GLFW_RELEASE)
            g_keys[key] = false;
    }
}

/* 控制相机运动: 并且，同时可以多个方向键生效*/
void do_movement()
{
    // 摄像机控制
    //GLfloat cameraSpeed = 5.0f * g_deltaTime;
    if (g_keys[GLFW_KEY_W])
        camera.ProcessKeyboard(Camera_Movement::FORWARD, g_deltaTime);
    //g_cameraPos += cameraSpeed * g_cameraFront;
    if (g_keys[GLFW_KEY_S])
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, g_deltaTime);
    if (g_keys[GLFW_KEY_A])
        camera.ProcessKeyboard(Camera_Movement::LEFT, g_deltaTime);
    if (g_keys[GLFW_KEY_D])
        camera.ProcessKeyboard(Camera_Movement::RIGHT, g_deltaTime);
}


/*鼠标控制yaw偏航角和pitch俯仰角*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static GLfloat lastX = 400, lastY = 300;
    static bool firstMouse = true;/*鼠标第一次进入窗口，先更新到屏幕中心,避免第一帧offset过大，画面跳动*/
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;/*注意鼠标屏幕坐标的y方向是朝下的, 而xyz坐标方向的y是朝上的*/
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}


/*通过鼠标的滚轮来缩放物体*/
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}