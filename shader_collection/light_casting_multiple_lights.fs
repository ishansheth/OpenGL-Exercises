#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 


struct DirLight 
{
     vec3 direction;
  
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
};

struct PointLight
{
     vec3 LightColor;
     vec3 position;
  
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
  
     float constant;
     float linear;
     float quadratic;
};

struct SpotLight
{
     vec3 position;
     vec3 direction;
     float cutoff;
     float outercutoff;
     
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
  
     float constant;
     float linear;
     float quadratic;
};

#define NR_POINT_LIGHTS 4
  
in vec3 Normal;
in vec3 fragpos;
in vec2 TexCoord;
  
uniform DirLight dirLight;
uniform SpotLight spotLight;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewpos;
uniform Material material;

//uniform vec3 LightColor[NR_POINT_LIGHTS];

uniform vec3 DirLightColor;

uniform bool is_dir_light;

uniform int nr_lights;

uniform int light_intensity;

out vec4 color;

vec2 getLightEffectValues(vec3 lightDirection,vec3 normal,vec3 viewDir)
{
	vec3 norm = normalize(normal);
     	vec3 lightDir = normalize(lightDirection);
     	float diff = max(dot(norm, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, norm);  
     	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 

	return vec2(diff,spec);
}

vec3 calcDirLight(DirLight light,vec3 normal,vec3 viewDir)
{
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoord));

	vec2 light_diff_spec = getLightEffectValues(light.direction,normal,viewDir);
	
     	vec3 diffuse = light.diffuse * light_diff_spec.x * vec3(texture(material.diffuse,TexCoord));
  
     	vec3 specular = light.specular * light_diff_spec.y * vec3(texture(material.specular,TexCoord)); 
  
	return (ambient + diffuse + specular);
}
  
  
vec3 calcPointLights(PointLight light, vec3 Normal, vec3 fragpos, vec3 viewDir)
{
	float distance = length(light.position-fragpos);
  	float attenuation =  light_intensity/(light.constant + light.linear * distance + light.quadratic * distance * distance);
  
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoord));
  	ambient *= attenuation;

	vec2 light_diff_spec = getLightEffectValues(light.position-fragpos,Normal,viewDir);

  	vec3 diffuse = light.diffuse * light_diff_spec.x * vec3(texture(material.diffuse,TexCoord));
  	diffuse *= attenuation;
  
  	vec3 specular = light.specular * light_diff_spec.y * vec3(texture(material.specular,TexCoord)); 
  	specular *= attenuation;
  
	return (ambient + diffuse + specular);
}

vec3 calcSpotLights(SpotLight light, vec3 normal, vec3 fragpos, vec3 viewDir)
{
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoord));
 
	vec3 norm = normalize(Normal);
  	vec3 lightDir = normalize(light.position - fragpos);
  	float diff = max(dot(norm, lightDir), 0.0);
  	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoord));
  
  	vec3 reflectDir = reflect(-lightDir, norm);
  	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoord)); 
    
	float theta = dot(lightDir,normalize(-light.direction)); 
  	float epsilon = light.cutoff - light.outercutoff;
  	float intensity = clamp((theta - light.outercutoff)/epsilon, 0.0, 1.0);

  	diffuse *= intensity;
  	specular *= intensity;

  	float distance = length(light.position-fragpos);
 	float attenuation =  1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
  
	diffuse *= attenuation;
  	specular *= attenuation;
  	ambient *= attenuation;
  
	vec3 result = (ambient + diffuse + specular);
	return result;
}


void main()
{
	vec3 norm = normalize(Normal);
  	vec3 viewDir = normalize(viewpos - fragpos);

  	vec3 result = vec3(0.0);
	if(is_dir_light)
		vec3 result = calcDirLight(dirLight,norm,viewDir)*DirLightColor;

	if(nr_lights < NR_POINT_LIGHTS)
	{
		for(int i = 0;i < nr_lights; i++)
     		{
			result += calcPointLights(pointLights[i],norm,fragpos,viewDir)*pointLights[i].LightColor;
     		}
	}
	else
	{
		for(int i = 0;i < NR_POINT_LIGHTS; i++)
     		{
			result += calcPointLights(pointLights[i],norm,fragpos,viewDir)*pointLights[i].LightColor;
     		}

	}
	result += calcSpotLights(spotLight,norm,fragpos,viewDir);
	
  	color = vec4(result , 1.0);
}
