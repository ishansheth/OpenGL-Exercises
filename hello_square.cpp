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

//#define NDEBUG
#include <cassert>

const unsigned int win_height = 600;
const unsigned int win_width = 800;

void framebuffer_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// opengl Error handling
void GLClearError()
{
  while(glGetError() != GL_NO_ERROR);
}

bool GLCheckError()
{
  while(GLenum error = glGetError())
    {
      std::cout<<" [OpenGL Error] (" << std::hex << error << ")" << std::endl;
      return false;
    }
  return true;
}

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

// opengl Error handling


std::string vertexshader =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) in vec3 position;"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position.x,position.y,position.z,1.0);\n"
    "}\n";

std::string vertexshader_with_color =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) in vec3 position;"
    "layout(location = 1) in vec3 acolor;"
							     
    "out vec3 vertexColor;"
							     
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position.x,position.y,position.z,1.0);\n"
    "    vertexColor = acolor;\n"							     
    "}\n";


std::string vertexshader_with_texture =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) in vec3 position;"
    "layout(location = 1) in vec2 texCoord;"
							     
    "out vec2 v_TexCoord;"
							     
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position.x,position.y,position.z,1.0);\n"
    "    v_TexCoord = texCoord;\n"							     
    "}\n";


std::string red_fragmentshader =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
    "}\n";

std::string blue_fragmentshader =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = vec4(0.2f, 0.3f, 0.8f, 1.0f);\n"
    "}\n";


std::string vertex_output_shader =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) in vec3 position;\n"
							     
    "out vec4 vertexColor;\n"
							     
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position.x,position.y,position.z,1.0);\n"
    "    vertexColor = vec4(0.5, 0.0, 0.0, 0.0);\n"							     
    "}\n";

std::string fragment_in_shader1 =
    "#version 330 core \n"
    "\n"
							     
    "uniform vec4 vertexColor;\n"
							     
    "out vec4 color;\n"
							     
    "void main()\n"
    "{\n"
    "    color = vertexColor;\n"
    "}\n";

std::string fragment_in_shader2 =
    "#version 330 core \n"
    "\n"
							     							     
    "out vec3 color;\n"

    "in vec3 vertexColor;\n"							     
							     
    "void main()\n"
    "{\n"
    "    color = vertexColor;\n"
    "}\n";


std::string fragmentshader_with_texture =
    "#version 330 core \n"
    "\n"
							     
    "out vec4 color;\n"							     
    "uniform sampler2D u_Texture;\n"
							     
    "in vec2 v_TexCoord;\n"							     
							     
    "void main()\n"
    "{\n"
    "    vec4 texColor = texture(u_Texture, v_TexCoord);\n"							     
    "    color = texColor;\n"
    "}\n";



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
  0.25f,-0.25f,0.0f, 0.0f,0.5f, // 0
  0.75f,-0.25f,0.0f, 0.5f,0.5f, // 1
  0.75f,-0.75f,0.0f, 0.5f,0.0f, // 2
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

  // rectangle 1 start
  VertexArray VAO1;  
  VertexBuffer vb1(vertices1,sizeof(vertices1));

  VertexBufferLayout vb_layout1;
  vb_layout1.Push<float>(3);

  VAO1.addBuffer(vb1,vb_layout1);
  
  IndexBuffer ib1(indices,6);
  
  // rectangle 1 end

  // rectangle 2 start
  VertexArray VAO2;
  VertexBuffer vb2(vertices2,sizeof(vertices2));

  VertexBufferLayout vb_layout2;
  vb_layout2.Push<float>(3);

  VAO2.addBuffer(vb2,vb_layout2);
    
  IndexBuffer ib2(indices,6);

  // rectangle 2 end

  // rectangle 3 start
  
  VertexArray VAO3;
  VertexBuffer vb3(vertices_with_color,sizeof(vertices_with_color));

  VertexBufferLayout vb_layout3;
  vb_layout3.Push<float>(3);
  vb_layout3.Push<float>(3);

  VAO3.addBuffer(vb3,vb_layout3);

  IndexBuffer ib3(indices,6);
  
  // rectangle 3 end

  // rectangle 4 start

  VertexArray VAO4;
  VertexBuffer vb4(vertices_with_texture,sizeof(vertices_with_texture));

  VertexBufferLayout vb_layout4;
  vb_layout4.Push<float>(3);
  vb_layout4.Push<float>(2);

  VAO4.addBuffer(vb4,vb_layout4);

  Texture tex1("awesomeface.png");
  IndexBuffer ib4(indices,6);

  // rectangle 4 end

  // shader collection start
  Shader shaders1(vertexshader,red_fragmentshader);

  Shader shaders2(vertexshader,blue_fragmentshader);

  Shader shaders3(vertexshader,fragment_in_shader1);

  Shader shaders4(vertexshader_with_color,fragment_in_shader2);

  Shader shaders5(vertexshader_with_texture,fragmentshader_with_texture);
  // shader collection end

  // renderer instance
  Renderer sq_renderer;
  
  glBindVertexArray(0);
  
  while(!glfwWindowShouldClose(window))
    {
      processInput(window);
      sq_renderer.clearWindow();
      
      float timeValue1 = glfwGetTime();
      float redValue = (cos(timeValue1) / 2.0f) + 0.5f;

      // draw first square
      sq_renderer.draw(VAO1,ib1,shaders1);

      // draw second square
      sq_renderer.setShaderUniform("vertexColor", shaders3, 0.0f, redValue, 0.0f, 1.0f);
      sq_renderer.draw(VAO2,ib2,shaders3);
      
      // draw third square
      sq_renderer.draw(VAO3,ib3,shaders4);

      // draw fourth square
      tex1.bind();
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
