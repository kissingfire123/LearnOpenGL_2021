#version 330 core

in vec2 TexCoord;

out vec4 color; 

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform int textureAlpha;

void main() 
{
    /*纹理有4种选择,下面任选一种放出来可以看到效果*/

	/*效果1: 纯色的矩形*/
 	//color = vec4(1.0f,0.5f,0.2f,1.0f); 

	/*效果2: 加载1个纹理texture1, 一个木箱子*/
	//color = texture(ourTexture1, TexCoord);

	/*效果3: 加载2个纹理texture1和texture2,且纹理2的透明度可调*/
	color = mix(texture(ourTexture1,TexCoord),texture(ourTexture2,vec2(1-TexCoord.x,TexCoord.y)),textureAlpha/100.0);//textureAlpha/100.0 means 2nd texture rate
}