#version 330 core
  
out vec4 color;

uniform vec3 LightColor;

void main()
{
    color = vec4(LightColor,1.0);
}
