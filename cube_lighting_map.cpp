#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "shader.hpp"
#include "Renderer.h"
#include "Texture.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int win_height = 600;
const unsigned int win_width = 800;

void framebuffer_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}


std::string vertexshader =
"#version 330 core \n"

"layout(location=0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec3 aTexCoord;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"out vec3 Normal;\n"
"out vec3 fragpos;\n"
"out vec3 TexCoord;\n"
  
"void main()\n"
"{\n"
   
"    gl_Position = projection * view * model * vec4(aPos,1.0);"
"    fragpos = vec3(model * vec4(aPos,1.0));\n"
"    Normal = aNormal;\n"
"    TexCoord = aTexCoord;\n"  
"}\n";


std::string fragmentshader =

"#version 330 core\n"
    "struct Material\n"
    "{\n"
    "   sampler2D diffuse;\n"
    "   vec3 specular;\n"
    "   float shininess;\n"
    "};\n"

    "struct Light \n"
    "{\n"
    "   vec3 position;\n"
  
    "   vec3 ambient;\n"
    "   vec3 diffuse;\n"
    "   vec3 specular;\n"
    "};\n"

    "uniform Light light;\n"    
    "uniform Material material;\n"
  
    "in vec3 Normal;\n"
    "in vec3 fragpos;\n"
  
    "out vec4 color;\n"
    "uniform vec3 lightpos;\n"
    "uniform vec3 viewpos;\n"
  
    "uniform vec3 objectColor;\n"
    "uniform vec3 lightColor;\n"


"void main()"
"{"
   " vec3 ambient = light.ambient * material.ambient;"
  	
   " vec3 norm = normalize(Normal);"
   " vec3 lightDir = normalize(lightpos - fragpos);"
   " float diff = max(dot(norm, lightDir), 0.0);"
   " vec3 diffuse = light.diffuse * ( diff * material.diffuse);"
    
   " vec3 viewDir = normalize(viewpos - fragpos);"
   " vec3 reflectDir = reflect(-lightDir, norm);  "
   " float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); "
   " vec3 specular = light.specular * (spec * material.specular); " 
        
   " vec3 result = (ambient + diffuse + specular);"
   " color = vec4(result, 1.0);"
  "}" ;


std::string vertexshader_lightsource =
"#version 330 core \n"

"layout(location=0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
  
"void main()\n"
"{\n"
   
"    gl_Position = projection * view * model * vec4(aPos,1.0);"
"}\n";


std::string fragmentshader_lightsource =
    "#version 330 core \n"
    "\n"
							     
    "out vec4 color;\n"							     
    "uniform vec3 lightColor;"
  
    "void main()\n"
    "{\n"
    "    color = vec4(lightColor, 1.0f);\n"
    "}\n";



int main()
{

}
