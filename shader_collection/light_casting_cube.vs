#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragpos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool is_room;

void main()
{
    fragpos = vec3(model * vec4(aPos, 1.0));

    if(is_room)
        Normal = mat3(transpose(inverse(model))) * (-aNormal);
    else
        Normal = mat3(transpose(inverse(model))) * aNormal;	
	
    TexCoord = aTexCoords;
    
    gl_Position = projection * view * vec4(fragpos, 1.0);
}

