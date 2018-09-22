#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"shader.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window,double xpos,double ypos);
void scroll_callback(GLFWwindow* window,double xoffset,double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);


bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){

  // initialize glfw library
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


  // create & initialize GLFW window
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT, "LearnOpenGL",NULL,NULL);

  if(window == NULL){
    std::cout<<"Failed to create GLFW window"<<std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
  glfwSetCursorPosCallback(window,mouse_callback);
  glfwSetScrollCallback(window,scroll_callback);

  glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
  
  glEnable(GL_DEPTH_TEST);

  if(glewInit() != GLEW_OK){
    fprintf(stderr,"Failed to initialize GLEW \n");
    getchar();
    glfwTerminate();
    return -1;
  }

  Shader ourShader("cameraPos.vs","cameraPos.fs");

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

  float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f    
  };

  unsigned int VBO,VAO;

  glGenVertexArrays(1,&VAO);   // generate vertex array
  glGenBuffers(1,&VBO);        // generate vertex buffer

  glBindVertexArray(VAO);      // bind vertex array

  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);


  // texture generation and setting its characteristic
  unsigned int texture1,texture2;

  // generate texture1 and bind it
  glGenTextures(1,&texture1);
  glBindTexture(GL_TEXTURE_2D,texture1);

  // set the parameters of texture wrapping
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  // set the parameters of texture filering
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  int width,height,nrChannels;
  stbi_set_flip_vertically_on_load(true);

  // load the image in a buffer
  unsigned char* data = stbi_load("brickwall.jpg",&width,&height,&nrChannels,0);
  if(data){
    
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }else{
    std::cout<<"failed to load texture"<<std::endl;
  }

  stbi_image_free(data);

  // generate texture2 and bind it
  glGenTextures(1,&texture2);
  glBindTexture(GL_TEXTURE_2D,texture2);

  // set the parameters of texture wrapping
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  // set the parameters of texture filering
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  // load the image in a buffer
  data = stbi_load("awesomeface.png",&width,&height,&nrChannels,0);
  if(data){    
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }else{
    std::cout<<"failed to load texture"<<std::endl;
  }
  stbi_image_free(data);

  ourShader.use();
  ourShader.setInt("texture1",0);
  ourShader.setInt("texture2",1);

  // passing the projection matrix early because it does not change 
  
  while(!glfwWindowShouldClose(window)){

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    processInput(window);

    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texture2);

    ourShader.use();
    
    glm::mat4 projection = glm::perspective(glm::radians(fov),(float)SCR_WIDTH / (float)SCR_HEIGHT,0.1f,100.0f);
    ourShader.setMat4("projection",projection);

    glm::mat4 view(1.0f);
    float radius = 10.0f;

    float camx = sin(glfwGetTime())* radius;
    float camz = cos(glfwGetTime())* radius;

    view = glm::lookAt(cameraPos,cameraPos + cameraFront, cameraUp);

    ourShader.setMat4("view",view);

    glBindVertexArray(VAO);
    
    for(unsigned int i = 0; i<10;i++){      
      glm::mat4 model(1.0f);
      model = glm::translate(model,cubePosition[i]);
      float angle = 20.0f * i;

      model = glm::rotate(model,glm::radians(angle),glm::vec3(1.0f,0.3f,0.5f));
      ourShader.setMat4("model",model);

      glDrawArrays(GL_TRIANGLES,0,36);      
    }

    glfwSwapBuffers(window);
    glfwPollEvents();

  }
  
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();
  return 0;

}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
      cameraPos += cameraSpeed * cameraFront;

    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= cameraSpeed * cameraFront;

    if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
      cameraPos -= glm::normalize(glm::cross(cameraFront,cameraUp)) * cameraSpeed;

    if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
      cameraPos += glm::normalize(glm::cross(cameraFront,cameraUp)) * cameraSpeed;      
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window,double xpos, double ypos){
  if(firstMouse)
    {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if(pitch > 89.0f)
    pitch = 89.0f;

  if(pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 front;

  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

  cameraFront = glm::normalize(front);
}


void scroll_callback(GLFWwindow* window,double xoffset, double yoffset){
  if(fov >= 1.0f && fov <= 45.0f)
    fov -= yoffset;

  if(fov <= 1.0f)
    fov = 1.0f;

  if(fov >= 45.0f)
    fov = 45.0f;
}
