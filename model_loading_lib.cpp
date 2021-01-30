#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.h"
#include <vector>

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


std::string vertexshader =
"#version 330 core \n"

"layout(location=0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aTexcoord;\n"


"out vec3 Normal;\n"
"out vec2 TexCoord;\n"
  
"void main()\n"
"{\n"
   
"    gl_Position = vec4(aPos,1.0);"
"    Normal = aNormal;"
"    TexCoord = aTexcoord;"
"}\n";


std::string fragmentshader =
"#version 330 core \n"

"in vec3 Normal;"
"in vec2 TexCoord;"

"out vec4 color;\n"

"uniform sampler2D texture_diffuse1;\n"
"uniform sampler2D texture_specular1;\n"
"uniform sampler2D texture_height1;\n"
"uniform sampler2D texture_normal1;\n"
  
"void main()\n"
"{\n"

  "color = texture(texture_diffuse1, TexCoord);"
  
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
  //glfwSetCursorPosCallback(window, mouse_callback);
  //glfwSetScrollCallback(window, scroll_callback);
    
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
  
  Model cubeModel("/home/ishan/opengl_programs/backpack/backpack.obj",false);

  std::cout<<"Number of meshes:"<<cubeModel.getNumMeshes()<<std::endl;
  
  Shader sh(vertexshader, fragmentshader);

  while(!glfwWindowShouldClose(window))
    {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      processInput(window);

      cubeModel.Draw(sh);
      glfwSwapBuffers(window);      
      glfwPollEvents();
      
    }
  
  glfwTerminate();
  return 0;

}

void processInput(GLFWwindow* window)
{
}
  
void framebuffer_callback(GLFWwindow* window, int width, int height)
{
}
