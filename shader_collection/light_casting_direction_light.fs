#version 330 core
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 position;
  
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
  
        float constant;
        float linear;
        float quadratic;
};

uniform Light light; 
uniform Material material;
uniform vec3 viewpos;
  
in vec3 Normal;
in vec3 fragpos;
in vec2 TexCoord;
  
out vec4 color;
  
void main()
{
	float distance = length(light.position-fragpos);
	float attenuation =  1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
  
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoord));
	ambient *= attenuation;
  
	vec3 norm = normalize(Normal);
    	vec3 lightDir = normalize(light.position - fragpos);
    	float diff = max(dot(norm, lightDir), 0.0);
    	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoord));
    	diffuse *= attenuation;
  
	vec3 viewDir = normalize(viewpos - fragpos);
    	vec3 reflectDir = reflect(-lightDir, norm);  
    	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 
    	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoord)); 
    	specular *= attenuation;
  
	vec3 result = (ambient + diffuse + specular);
    	color = vec4(result, 1.0);
}
