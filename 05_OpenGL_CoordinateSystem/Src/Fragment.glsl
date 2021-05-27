#version 330 core


in vec3 ourColor;
in vec2 TexCoord;

out vec4 color; 

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform int textureAlpha;

void main() 
{
 	//color = vec4(1.0f,0.5f,0.2f,1.0f); 
	//color = texture(ourTexture, TexCoord);
	//color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
	color = mix(texture(ourTexture1,TexCoord),texture(ourTexture2,vec2(1-TexCoord.x,TexCoord.y)),textureAlpha/100.0);//0.2means 2nd texture rate
}