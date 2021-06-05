#version 330 core 
layout (location = 0) in vec3 position; /*从VBO接收到的数据,并自主命名*/
layout (location = 1) in vec3 color;

out vec3 vertexSelfColor;/*把每个vertex的数据,向fragment-Shader输出一个color*/
void main() 
{
      /*按照原始坐标正常显示*/
    //gl_Position = vec4(position.x,position.y,position.z,1.0); 
      
      /*y取相反数, 上下翻转,平行四边形改变朝向*/
    gl_Position = vec4(position.x, - position.y,position.z,1.0); 

    vertexSelfColor = color;
} 