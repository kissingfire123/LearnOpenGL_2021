#version 330 core 
layout (location = 0) in vec3 position; /*��VBO���յ�������,����������*/
layout (location = 1) in vec3 color;

out vec3 vertexSelfColor;/*��ÿ��vertex������,��fragment-Shader���һ��color*/
void main() 
{
      /*����ԭʼ����������ʾ*/
    //gl_Position = vec4(position.x,position.y,position.z,1.0); 
      
      /*yȡ�෴��, ���·�ת,ƽ���ı��θı䳯��*/
    gl_Position = vec4(position.x, - position.y,position.z,1.0); 

    vertexSelfColor = color;
} 