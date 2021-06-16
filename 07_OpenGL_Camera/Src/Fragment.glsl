#version 330 core

in vec2 TexCoord;

out vec4 color; 

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform int textureAlpha;

void main() 
{
    /*������4��ѡ��,������ѡһ�ַų������Կ���Ч��*/

	/*Ч��1: ��ɫ�ľ���*/
 	//color = vec4(1.0f,0.5f,0.2f,1.0f); 

	/*Ч��2: ����1������texture1, һ��ľ����*/
	//color = texture(ourTexture1, TexCoord);

	/*Ч��3: ����2������texture1��texture2,������2��͸���ȿɵ�*/
	color = mix(texture(ourTexture1,TexCoord),texture(ourTexture2,vec2(1-TexCoord.x,TexCoord.y)),textureAlpha/100.0);//textureAlpha/100.0 means 2nd texture rate
}