#version 330 core 

layout (location = 0) in vec3 position;/*接收来自VBO的数据,并且自行命名*/
layout (location = 1) in vec2 texCoord;


out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    /*  矩阵生效注意从右向左读  */
     gl_Position = projection * view * model * vec4(position, 1.0f); 
    TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);
} 