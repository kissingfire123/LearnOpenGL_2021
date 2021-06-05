#version 330 core
out vec4 color;

in vec3 vertexSelfColor; /*��Vertex-shader���յ�color*/

uniform vec4 uniformColor;
void main() 
{
     /*����ѡ�������ѡһ�֣�����Ч��*/

    /*ѡ��1��ʹ�ù̶���ɫ*/
 	//color = vec4(1.0f,0.5f,0.2f,1.0f); //whole frame with single color
   
   /*ѡ��2��ʹ��ȫ�ֱ���ʽ,��CPUָ�����ݵ���ɫ,����ʵ�ֱ任Ч��*/
    //color = uniformColor;

   /*ѡ��3��4������ʹ�ø��Ե���ɫ���м�λ����shader�Զ���ֵ*/
    color= vec4(vertexSelfColor,1.0f );
    
}