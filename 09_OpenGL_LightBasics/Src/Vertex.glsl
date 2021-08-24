#version 330 core 



layout (location = 0) in vec3 position;/*��������VBO������,������������*/
layout (location = 1) in vec3 normal;/*��������VBO������,������������*/ 

out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    /*  ������Чע����������  */
     gl_Position = projection * view * model * vec4(position, 1.0f); 
    Normal = normal;
} 