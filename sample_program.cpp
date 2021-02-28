//#include<GL/freeglut.h>
//#include<GL/gl.h>


#include<GL/glew.h>
#include<GLFW/glfw3.h>

//#include<GL/glut.h>
#include<iostream>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <stdio.h>
//#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = "#version 330 core\n"
  "layout(location = 0) in vec3 aPos;\n"
  "void main()\n"
  "{\n"
  "    gl_Position.xyz = aPos;\n"
  "    gl_Position.w = 1.0"
  "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
       "FragColor = vec4(1.0f,0.5f,0.2f,1.0f);\n"
  "}\0";

static void error_callback(int error, const char *description)
{
    puts(description);
}


int main(int argc, char** argv){


  glfwSetErrorCallback(error_callback);

  if(!glfwInit()){
    fprintf(stderr,"Failed to initialize GLFW!!");
    getchar();
    return -1;
  }

  GLFWwindow* window = glfwCreateWindow(1024,768,"OpenGL window",NULL,NULL);
  if(window == NULL){
    fprintf(stderr,"Failed to open GLFW window!!");
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  
  glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);


  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
  if(!success){
    glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
    std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED"<<std::endl;
  }
  
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
  if(!success){
    glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
    std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED"<<std::endl;
  }

  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram,vertexShader);
  glAttachShader(shaderProgram,fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
  if(!success){
    glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
    std::cout<<"ERROR::SHADER::PROGRAM::LINKING_FAILED"<<std::endl;    
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  float vertices[] = {
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,0.5f,0.0f
  };

  unsigned int indices[] = {
    0,1,3,
    1,2,3
  };

  unsigned int VBO,VAO,EBO;
  
  glGenVertexArrays(1,&VAO);
  glGenBuffers(1,&VBO);
  glGenBuffers(1,&EBO);
  
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER,0);

  glBindVertexArray(0);

  std::cout<<"entering in the loop"<<std::endl;
  while(!glfwWindowShouldClose(window)){
    processInput(window);
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glDeleteVertexArrays(1,&VAO);
  glDeleteBuffers(1,&VBO);
  glDeleteBuffers(1,&EBO);
  
  glfwTerminate();

  return 0;
}

void processInput(GLFWwindow* window){

    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window,true);

}

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
  glViewport(0,0,width,height);
}
