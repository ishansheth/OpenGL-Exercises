#include <iostream>
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
"out vec3 fragpos;\n"
  
"void main()\n"
"{\n"
   
"    gl_Position = projection * view * model * vec4(aPos,1.0);"
"    fragpos = vec3(model * vec4(aPos,1.0));\n"
"    Normal = aNormal;\n"
"}\n";


std::string fragmentshader =

"#version 330 core\n"
    "struct Material\n"
    "{\n"
    "   vec3 ambient;\n "
    "   vec3 diffuse;\n"
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
   " vec3 reflectDir = reflect(-lightDir, norm);"
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

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = win_width / 2.0f;
float lastY = win_height / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


std::vector<Vertex> createVertexData()
{
  std::vector<Vertex> vBuffer;
  for(unsigned int i = 0;i < sizeof(cube_vertices); i = i + 6)
    {
      Vertex v;
      v.position = { cube_vertices[i], cube_vertices[i+1], cube_vertices[i+2] };
      v.isPos = true;
      v.normal = { cube_vertices[i+3], cube_vertices[i+4], cube_vertices[i+5] };
      v.isNormal = true;
      vBuffer.push_back(v);
    }
  return vBuffer;
}

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

  ShaderCollection shader_library;
  VertexArray VAO4;
  VertexBuffer vb4(cube_vertices,sizeof(cube_vertices));

  VertexBufferLayout vb_layout4;
  vb_layout4.Push<float>(3);
  vb_layout4.Push<float>(3);

  VAO4.addBuffer(vb4,vb_layout4);

  shader_library.addShader("main_cube", new Shader(vertexshader, fragmentshader));
  shader_library.addShader("light_source", new Shader(vertexshader_lightsource, fragmentshader_lightsource));
  
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
      lightPos = glm::vec3(camX, 0.0f, camZ);

      glm::mat4 view = camera.GetViewMatrix();
      
      glm::vec3 lightColor;
      lightColor.x = sin(glfwGetTime() * 2.0f);
      lightColor.y = sin(glfwGetTime() * 0.7f);
      lightColor.z = sin(glfwGetTime() * 1.3f);

      glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
      glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
      //glm::mat4 view = glm::lookAt(glm::vec3(-5.0f,5.0f,5.0f),glm::vec3(0.0f,0.0f,0.0f) , glm::vec3(-1.0f,0.0f,0.0f));

      shader_library.getShader("main_cube")->bind();
      shader_library.getShader("main_cube")->setMat4("model",model);      
      shader_library.getShader("main_cube")->setMat4("view",view);      
      shader_library.getShader("main_cube")->setMat4("projection",projection);
      
      shader_library.getShader("main_cube")->setVec3("lightpos",lightPos);
      shader_library.getShader("main_cube")->setVec3("viewpos",camera.Position);
      shader_library.getShader("main_cube")->setVec3("objectColor",glm::vec3(1.0f, 0.5f, 0.31f));
      shader_library.getShader("main_cube")->setVec3("lightColor",lightColor);

      shader_library.getShader("main_cube")->setVec3("material.ambient",glm::vec3(1.0f,0.5f,0.31f));
      shader_library.getShader("main_cube")->setVec3("material.diffuse",glm::vec3(1.0f,0.5f,0.31f));
      shader_library.getShader("main_cube")->setVec3("material.specular",glm::vec3(0.5f,0.5f,0.5f));
      shader_library.getShader("main_cube")->setFloat("material.shininess",32.0f);

      shader_library.getShader("main_cube")->setVec3("light.ambient", ambientColor);
      shader_library.getShader("main_cube")->setVec3("light.diffuse", diffuseColor);
      shader_library.getShader("main_cube")->setVec3("light.specular", glm::vec3(1.0f,1.0f,1.0f));

      sq_renderer.drawArray(VAO4,*(shader_library.getShader("main_cube")),36);

      model = glm::mat4(1.0f);
      model = glm::translate(model, lightPos);
      model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

      
      shader_library.getShader("light_source")->bind();
      shader_library.getShader("light_source")->setMat4("model",model);      
      shader_library.getShader("light_source")->setMat4("view",view);      
      shader_library.getShader("light_source")->setMat4("projection",projection);
      shader_library.getShader("light_source")->setVec3("lightColor",lightColor);

      sq_renderer.drawArray(VAO4,*(shader_library.getShader("light_source")),36);
      
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

