#version 330 core
out vec4 color;

in vec3 vertexSelfColor; /*从Vertex-shader接收的color*/

uniform vec4 uniformColor;
void main() 
{
     /*以下选择可以任选一种，看下效果*/

    /*选择1：使用固定颜色*/
 	//color = vec4(1.0f,0.5f,0.2f,1.0f); //whole frame with single color
   
   /*选择2：使用全局变量式,从CPU指定传递的颜色,用于实现变换效果*/
    //color = uniformColor;

   /*选择3：4个顶点使用各自的颜色，中间位置让shader自动插值*/
    color= vec4(vertexSelfColor,1.0f );
    
}