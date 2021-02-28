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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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


float cube_vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };


glm::vec3 cubePosition[] = {
      glm::vec3(0.0f,0.0f,0.0f),
      glm::vec3(2.0f,5.0f,-15.0f),
      glm::vec3(-1.5f,-2.2f,-2.5f),
      glm::vec3(-3.8f,-2.0f,-12.3f),
      glm::vec3(2.4f,-0.4f,-3.5f),
      glm::vec3(-1.7f,3.0f,-7.5f),
      glm::vec3(1.3f,-2.0f,-2.5f),
      glm::vec3(1.5f,2.0f,-2.5f),
      glm::vec3(1.5f,0.2f,-1.5f),
      glm::vec3(-1.3f,1.0f,-1.5f)
};

glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = win_width / 2.0f;
float lastY = win_height / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool dir_light = true;

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
  vb_layout4.Push<float>(2);

  VAO4.addBuffer(vb4,vb_layout4);

  Texture diffuseMap("texture_collection/container2.png");
  Texture specularMap("texture_collection/container2_specular.png");

  ShaderCollection shader_library;
  shader_library.addShader("multiplelights", new Shader("shader_collection/light_casting_cube.vs", "shader_collection/light_casting_multiple_lights.fs"));
  
  shader_library.addShader("light_source", new Shader("shader_collection/light_casting_lightsource.vs", "shader_collection/light_casting_lightsource.fs"));

  shader_library.getShader("multiplelights")->bind();
  shader_library.getShader("multiplelights")->setUniformValue(glUniform1i,"material.diffuse",0);
  shader_library.getShader("multiplelights")->setUniformValue(glUniform1i,"material.specular",1);

  Renderer sq_renderer;

  const char* glsl_version = "#version 330";
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
 
  glm::vec3 lightPos;

  float* light_color = new float[4];
    
  glEnable(GL_DEPTH_TEST);
  
  while(!glfwWindowShouldClose(window))
    {      
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      processInput(window);
      sq_renderer.clearWindow();
      glClear(GL_DEPTH_BUFFER_BIT);

      float currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      const float radius = 5.0f;
      float camX = sin(glfwGetTime()) * radius;
      float camZ = cos(glfwGetTime()) * radius;
      lightPos = glm::vec3(camX, 0.0f, camZ);

      diffuseMap.bind(0);
      specularMap.bind(1);
      
      glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f/600.0f, 0.1f, 100.0f);
      glm::mat4 view = camera.GetViewMatrix();

      
      shader_library.getShader("multiplelights")->bind();
      
      shader_library.getShader("multiplelights")->setUniformValue(glUniformMatrix4fv,"view",1,GL_FALSE,&view[0][0]);      
      shader_library.getShader("multiplelights")->setUniformValue(glUniformMatrix4fv,"projection",1,GL_FALSE,&projection[0][0]);
      
      shader_library.getShader("multiplelights")->setVec3("viewpos",camera.Position);
      shader_library.getShader("multiplelights")->setVec3("LightColor",glm::vec3(light_color[0],light_color[1],light_color[2]));

      shader_library.getShader("multiplelights")->setFloat("material.shininess",64.0f);

      shader_library.getShader("multiplelights")->setVec3("dirLight.direction",-0.2f,-1.0f,-0.3f);
      shader_library.getShader("multiplelights")->setVec3("dirLight.ambient",0.05f,0.05f,0.05f);      
      shader_library.getShader("multiplelights")->setVec3("dirLight.diffuse",0.4f,0.4f,0.4f);
      shader_library.getShader("multiplelights")->setVec3("dirLight.specular",0.5f,0.5f,0.5f);
      shader_library.getShader("multiplelights")->setBool("is_dir_light",dir_light);
      

      for(int i = 0; i < 4; i++)
	{
	  std::string pointlight_index = "pointLights[" + std::to_string(i) + "]";
	  shader_library.getShader("multiplelights")->setVec3(pointlight_index + ".position",pointLightPositions[i]);
	  shader_library.getShader("multiplelights")->setVec3(pointlight_index + ".ambient",0.05f,0.05f,0.05f);
	  shader_library.getShader("multiplelights")->setVec3(pointlight_index + ".diffuse",0.8f,0.8f,0.8f);
	  shader_library.getShader("multiplelights")->setVec3(pointlight_index + ".specular",1.0f,1.0f,1.0f);
	  shader_library.getShader("multiplelights")->setFloat(pointlight_index + ".constant",1.0f);
	  shader_library.getShader("multiplelights")->setFloat(pointlight_index + ".linear",0.09f);
	  shader_library.getShader("multiplelights")->setFloat(pointlight_index + ".quadratic",0.032f);	  	  
	}
            
      for(unsigned int i = 0; i<10;i++)
	{      
	  glm::mat4 model(1.0f);
	  model = glm::translate(model,cubePosition[i]);
	  float angle = 20.0f * i;
	  
	  model = glm::rotate(model,glm::radians(angle),glm::vec3(1.0f,0.3f,0.5f));
	  
	  shader_library.getShader("multiplelights")->setUniformValue(glUniformMatrix4fv,"model",1,GL_FALSE,&model[0][0]);   	  
	  sq_renderer.drawArray(VAO4,*(shader_library.getShader("multiplelights")),36);
	}
      
      for(int i = 0; i < 4; i++)
	{
	  glm::mat4 model(1.0f);
	  model = glm::translate(model, pointLightPositions[i]);
	  model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	  
	  shader_library.getShader("light_source")->bind();
	  shader_library.getShader("light_source")->setMat4("model",model);      
	  shader_library.getShader("light_source")->setMat4("view",view);      
	  shader_library.getShader("light_source")->setMat4("projection",projection);
	  shader_library.getShader("light_source")->setVec3("LightColor",glm::vec3(light_color[0],light_color[1],light_color[2]));
	  
	  sq_renderer.drawArray(VAO4,*(shader_library.getShader("light_source")),36);
	}

      /*ImGui Panel code -------START*/
      
      ImGui_ImplOpenGL3_NewFrame();      
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
	
      ImGui::Begin("Light Control Panel");
      ImGui::Checkbox("Direction Light", &dir_light); 
      ImGui::ColorEdit4("Light Color", light_color);
            
      ImGui::End();
      
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      /*ImGui Panel code -------END*/

      glfwSwapBuffers(window);      
      glfwPollEvents();

    }

  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  
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

