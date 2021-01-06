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

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"out vec3 Normal;\n"
"out vec3 fragPos;\n"
  
"void main()\n"
"{\n"
   
"    gl_Position = projection * view * model * vec4(aPos,1.0);"
"    fragPos = vec3(model * vec4(aPos,1.0));\n"
"    Normal = aNormal;"
"}\n";

std::string fragmentshader =
    "#version 330 core \n"
    "\n"

    "in vec3 Normal;\n"
    "in vec3 fragPos;\n"
    "out vec4 color;\n"

    "uniform vec3 lightpos;\n"
    "uniform vec3 objectColor;\n"
    "uniform vec3 lightColor;\n"  
							     
    "void main()\n"
    "{\n"
    "    float ambientStrength = 0.1;\n"
    "    vec3 ambient = lightColor * ambientStrength;\n"
  
    "    vec3 norm = normalize(Normal);\n"
    "    vec3 lightDir = (lightpos - fragPos);\n"
    "    float diff = max(dot(lightDir,norm),0.0);\n"
    "    vec3 diffuse = diff * lightColor;\n"
    "    vec3 result = (ambient + diffuse) * objectColor;\n"
  
    "    color = vec4(result, 1.0f);\n"
    "}\n";



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
							     
    "void main()\n"
    "{\n"
    "    color = vec4(1.0f);\n"
    "}\n";


float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

/*
float cube_vertices[] = {  
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
};


unsigned int indices[] = {
  0,1,2,
  2,3,0,

  4,5,6,
  6,7,4,

  8,9,10,
  10,11,8,

  12,13,14,
  14,15,12,

  16,17,18,
  18,19,16,

  20,21,22,
  22,23,20
};
*/

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = win_width / 2.0f;
float lastY = win_height / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* initGLFWAndCreateWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(win_width,win_height,"LearnOpenGL",NULL,NULL);

  if(window == NULL)
    {
      std::cout<<"Failed to create window"<<std::endl;
      glfwTerminate();
      return NULL;
    }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window,framebuffer_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
    
  // glewInit() must be called after the creation of opengl context
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return NULL;
  }

  return window;
}

int main()
{

  GLFWwindow* window = initGLFWAndCreateWindow();

  // opengl Error handling
  // During init, enable debug output
  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );
  // opengl Error handling
  
  if(window == NULL)
    return -1;
  else
    std::cout<<"OpenGL window and contex set"<<std::endl;

  VertexArray VAO4;
  VertexBuffer vb4(cube_vertices,sizeof(cube_vertices));

  VertexBufferLayout vb_layout4;
  vb_layout4.Push<float>(3);
  vb_layout4.Push<float>(3);

  VAO4.addBuffer(vb4,vb_layout4);

  Shader shaders1(vertexshader, fragmentshader);
  Shader shaders2(vertexshader_lightsource, fragmentshader_lightsource);
  
  // renderer instance
  Renderer sq_renderer;
  
  glm::vec3 lightPos;
  
  glEnable(GL_DEPTH_TEST);
  
  while(!glfwWindowShouldClose(window))
    {

      float currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;
	
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      processInput(window);
      sq_renderer.clearWindow();
      glClear(GL_DEPTH_BUFFER_BIT);

      glm::mat4 model = glm::mat4(1.0f);
      glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f/600.0f, 0.1f, 100.0f);

      const float radius = 5.0f;
      float camX = sin(glfwGetTime()) * radius;
      float camZ = cos(glfwGetTime()) * radius;
      lightPos = glm::vec3(camX, 0.0, camZ);

      glm::mat4 view = camera.GetViewMatrix();

      //glm::mat4 view = glm::lookAt(glm::vec3(0.0f,15.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f) , glm::vec3(-1.0f,0.0f,0.0f));

      shaders1.bind();
      shaders1.setMat4("model",model);      
      shaders1.setMat4("view",view);      
      shaders1.setMat4("projection",projection);
      shaders1.setVec3("lightpos",lightPos);
      shaders1.setVec3("objectColor",1.0f, 0.5f, 0.31f);
      shaders1.setVec3("lightColor",1.0f, 1.0f, 1.0f);
      
      sq_renderer.drawArray(VAO4,shaders1,36);

      model = glm::mat4(1.0f);
      model = glm::translate(model, lightPos);
      model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	
      shaders2.bind();
      shaders2.setMat4("model",model);      
      shaders2.setMat4("view",view);      
      shaders2.setMat4("projection",projection);

      sq_renderer.drawArray(VAO4,shaders2,36);
      
      glfwSwapBuffers(window);      
      glfwPollEvents();

    }

  glfwTerminate();
  return 0;

}

void processInput(GLFWwindow* window)
{
if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime); 
}

void framebuffer_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0,0,width,height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

