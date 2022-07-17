#version 330 core

out vec4 color; 

uniform vec3 lightPos; 
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

void main() 
{

	// calculate diffuse light
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max( dot(norm, lightDir), 0.0f );
	vec3 diffuse = diff * lightColor;

	// ambient light
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	// specular light
	float specularStength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), 32);//32 is shininess,反光度
	vec3 specular = specularStength * spec * lightColor;

	color = vec4((ambient + diffuse + specular)*objectColor,1.0f);
}
