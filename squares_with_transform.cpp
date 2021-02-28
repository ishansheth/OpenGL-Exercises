#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "shader.h"
#include "Renderer.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define NDEBUG
#include <cassert>

const unsigned int win_height = 600;
const unsigned int win_width = 800;

void framebuffer_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


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


float vertices1[] = {
    -0.25f,-0.25f,0.0f, // 0
    0.25f,-0.25f,0.0f, // 1
    0.25f,0.25f,0.0f, // 2
    -0.25f,0.25f,0.0f // 3
};

float vertices2[] = {
    0.25f,0.25f,0.0f, // 0
    0.25f,0.75f,0.0f, // 1
    0.75f,0.75f,0.0f, // 2
    0.75f,0.25f,0.0f // 3
};

float vertices_with_texture[] = {
  0.25f,-0.25f,0.0f, 0.0f,1.0f, // 0
  0.75f,-0.25f,0.0f, 1.0f,1.0f, // 1
  0.75f,-0.75f,0.0f, 1.0f,0.0f, // 2
  0.25f,-0.75f,0.0f, 0.0f,0.0f  // 3
};

float vertices_with_color[] = {
  -0.25f,-0.25f,0.0f, 1.0f, 0.0f, 0.0f,
  -0.25f,-0.75f,0.0f, 0.0f, 1.0f, 0.0f,
  -0.75f,-0.75f,0.0f, 0.0f, 0.0f, 1.0f,
  -0.75f,-0.25f,0.0f, 1.0f, 1.0f, 1.0f
};
  
unsigned int indices[] = {
    0,1,2,
    2,3,0
  };

std::string vertexshader_with_transform =
  "#version 330 core \n"
  "\n"
  "layout(location=0) in vec3 aPos;\n"
  "layout(location=1) in vec2 aTexcoord;\n"

  "out vec2 TexCoord;\n"
  
  "uniform mat4 transform;\n"
  
  "void main()\n"
  "{\n"
  "    gl_Position = transform * vec4(aPos,1.0f);\n"
  "    TexCoord = aTexcoord;\n"
  "}\n";

std::string fragmentshader_with_texture =
    "#version 330 core \n"
    "\n"
							     
    "out vec4 color;\n"							     
    "uniform sampler2D u_Texture1;\n"
    "uniform sampler2D u_Texture2;\n"
  
    "in vec2 TexCoord;\n"							     
							     
    "void main()\n"
    "{\n"
    "    vec4 texColor = mix(texture(u_Texture1, TexCoord),texture(u_Texture2, TexCoord),0.5);\n"
    "    color = texColor;\n"
    "}\n";


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
  VertexBuffer vb4(vertices_with_texture,sizeof(vertices_with_texture));

  VertexBufferLayout vb_layout4;
  vb_layout4.Push<float>(3);
  vb_layout4.Push<float>(2);

  VAO4.addBuffer(vb4,vb_layout4);
  IndexBuffer ib4(indices,6);

  Texture tex1("awesomeface.png");
  Texture tex2("brickwall.jpg");

  // rectangle 4 end

  Shader shaders5(vertexshader_with_transform, fragmentshader_with_texture);
  shaders5.bind();
  shaders5.setInt("u_Texture1", 0);
  shaders5.setInt("u_Texture2", 1);
  
  // renderer instance
  Renderer sq_renderer;
  
  while(!glfwWindowShouldClose(window))
    {
      processInput(window);
      sq_renderer.clearWindow();
      tex1.bind(0);
      tex2.bind(1);

      glm::mat4 transform = glm::mat4(1.0f);
      transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
      transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
      
      shaders5.bind();
      shaders5.setMat4("transform",transform);      
      
      sq_renderer.draw(VAO4,ib4,shaders5);
      
      glfwSwapBuffers(window);      
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;

}


void processInput(GLFWwindow* window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window,true);
}

void framebuffer_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0,0,width,height);
}

