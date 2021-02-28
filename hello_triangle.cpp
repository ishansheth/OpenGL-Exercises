#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

const unsigned int win_height = 600;
const unsigned int win_width = 800;

void framebuffer_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

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

unsigned int compileShader(unsigned int type, const std::string& source)
{
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(id,1,&src,nullptr);
  glCompileShader(id);

  int success;
  char infolog[512];
  glGetShaderiv(id,GL_COMPILE_STATUS,&success);
  if(!success)
    {
      glGetShaderInfoLog(id,512,NULL,infolog);
      std::cout<<infolog<<std::endl;
    }
  else
    {
      std::cout<<"shader compiled succesfully"<<std::endl;
    }
  return id;
}

unsigned int createShader(const std::string& vertexshader,const std::string& fragshader)
{
  unsigned int program = glCreateProgram();

  unsigned int vs = compileShader(GL_VERTEX_SHADER,vertexshader);
  unsigned int fs = compileShader(GL_FRAGMENT_SHADER,fragshader);

  glAttachShader(program,vs);
  glAttachShader(program,fs);
  glLinkProgram(program);

  int success;
  char infolog[512];

  glGetProgramiv(program,GL_LINK_STATUS,&success);
  if(!success)
    {
      glGetProgramInfoLog(program,512,NULL,infolog);
      std::cout<<infolog<<std::endl;
    }
  else
    {
      std::cout<<"program linked succesfully"<<std::endl;
    }

  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);
  
  return program;
}

int main()
{
  GLFWwindow* window = initGLFWAndCreateWindow();

  if(window == NULL)
    return -1;
  else
    std::cout<<"OpenGL window and contex set"<<std::endl;


  float vertices[] = {
    -0.5f,-0.5f,0.0f,
    0.5f,-0.5f,0.0f,
    0.0f,0.5f,0.0f,    
  };

  unsigned int VBO,VAO;
  glGenVertexArrays(1,&VAO);
  glBindVertexArray(VAO);
  
  glGenBuffers(1,&VBO);
  
  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
  glEnableVertexAttribArray(0);
  
  std::string vertexshader =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) in vec3 position;"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position.x,position.y,position.z,1.0);\n"
    "}\n";
  
  std::string fragmentshader =
    "#version 330 core \n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
    "}\n";


  unsigned int shaders = createShader(vertexshader,fragmentshader);
  glUseProgram(shaders);

  while(!glfwWindowShouldClose(window))
    {
      processInput(window);
      glClear(GL_COLOR_BUFFER_BIT);

      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES,0,3);
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
  std::cout<<"in resize callback"<<std::endl;
  glViewport(0,0,width,height);
}
