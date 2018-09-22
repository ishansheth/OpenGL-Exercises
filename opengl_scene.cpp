/**
TODO: come back to this later
In order to create scene here, more reading is required.
 **/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"shader.hpp"


void framebuffer_size_callback(GLFWwindow* window,int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;
float farPlaneDistance = -70.0f;

float rotationAngle;
bool rotate = true;

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Room",NULL,NULL);

    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    glEnable(GL_DEPTH_TEST);
    
    if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      getchar();
      glfwTerminate();
      return -1;
    }

    Shader ourShader("coordinateSystem.vs", "coordinateSystem.fs");

    Shader smallCubeShader("coloredCubeObject.vs","coloredCubeObject.fs");

    float cubeVertices[] = {
      // back side
      -20.5f, -20.5f, -50.5f,  0.0f, 0.0f,
      20.5f, -20.5f, -50.5f,  1.0f, 0.0f,
      20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      -20.5f,  20.5f, -50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 0.0f,

      //front side
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      20.5f, -20.5f,  50.5f,  1.0f, 0.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 1.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 1.0f,
      -20.5f,  20.5f,  50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,

      //left side
      -20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
      -20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      -20.5f,  20.5f,  50.5f,  1.0f, 0.0f,

      //right side
      20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
      20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 0.0f,

      //bottom side
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      20.5f, -20.5f, -50.5f,  1.0f, 1.0f,
      20.5f, -20.5f,  50.5f,  1.0f, 0.0f,
      20.5f, -20.5f,  50.5f,  1.0f, 0.0f,
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,

      //top side
      -20.5f,  20.5f, -50.5f,  0.0f, 1.0f,
      20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
      -20.5f,  20.5f,  50.5f,  0.0f, 0.0f,
      -20.5f,  20.5f, -50.5f,  0.0f, 1.0f
    };

    float smallcubevertices[] = {

      -1.5f, -1.5f, -15.5f, 1.0f,0.0f,0.0f, 0.0f, 0.0f,
      1.5f, -1.5f, -15.5f,  1.0f,0.0f,0.0f, 1.0f, 0.0f,
      1.5f,  1.5f, -15.5f,  1.0f,0.0f,0.0f, 1.0f, 1.0f,
      1.5f,  1.5f, -15.5f,  1.0f,0.0f,0.0f, 1.0f, 1.0f,
      -1.5f,  1.5f, -15.5f, 1.0f,0.0f,0.0f, 0.0f, 1.0f,
      -1.5f, -1.5f, -15.5f, 1.0f,0.0f,0.0f, 0.0f, 0.0f,

      //front side
      -1.5f, -1.5f,  15.5f, 0.0f,1.0f,0.0f, 0.0f, 0.0f,
      1.5f, -1.5f,  15.5f,  0.0f,1.0f,0.0f, 1.0f, 0.0f,
      1.5f,  1.5f,  15.5f,  0.0f,1.0f,0.0f, 1.0f, 1.0f,
      1.5f,  1.5f,  15.5f,  0.0f,1.0f,0.0f, 1.0f, 1.0f,
      -1.5f,  1.5f,  15.5f, 0.0f,1.0f,0.0f, 0.0f, 1.0f,
      -1.5f, -1.5f,  15.5f, 0.0f,1.0f,0.0f, 0.0f, 0.0f,

      //left side
      -1.5f,  1.5f,  15.5f, 0.0f,0.0f,1.0f, 1.0f, 0.0f,
      -1.5f,  1.5f, -15.5f, 0.0f,0.0f,1.0f, 1.0f, 1.0f,
      -1.5f, -1.5f, -15.5f, 0.0f,0.0f,1.0f,  0.0f, 1.0f,
      -1.5f, -1.5f, -15.5f, 0.0f,0.0f,1.0f,  0.0f, 1.0f,
      -1.5f, -1.5f,  15.5f, 0.0f,0.0f,1.0f,  0.0f, 0.0f,
      -1.5f,  1.5f,  15.5f, 0.0f,0.0f,1.0f,  1.0f, 0.0f,

      //right side
      1.5f,  1.5f,  15.5f, 1.0f,1.0f,0.0f,  1.0f, 0.0f,
      1.5f,  1.5f, -15.5f, 1.0f,1.0f,0.0f,  1.0f, 1.0f,
      1.5f, -1.5f, -15.5f, 1.0f,1.0f,0.0f,  0.0f, 1.0f,
      1.5f, -1.5f, -15.5f, 1.0f,1.0f,0.0f,  0.0f, 1.0f,
      1.5f, -1.5f,  15.5f, 1.0f,1.0f,0.0f,  0.0f, 0.0f,
      1.5f,  1.5f,  15.5f, 1.0f,1.0f,0.0f,  1.0f, 0.0f,

      //bottom side
      -1.5f, -1.5f, -15.5f, 0.0f,1.0f,1.0f,  0.0f, 1.0f,
      1.5f, -1.5f, -15.5f,  0.0f,1.0f,1.0f, 1.0f, 1.0f,
      1.5f, -1.5f,  15.5f,  0.0f,1.0f,1.0f, 1.0f, 0.0f,
      1.5f, -1.5f,  15.5f,  0.0f,1.0f,1.0f, 1.0f, 0.0f,
      -1.5f, -1.5f,  15.5f, 0.0f,1.0f,1.0f, 0.0f, 0.0f,
      -1.5f, -1.5f, -15.5f, 0.0f,1.0f,1.0f, 0.0f, 1.0f,

      //top side
      -1.5f,  1.5f, -15.5f, 1.0f,0.0f,1.0f, 0.0f, 1.0f,
      1.5f,  1.5f, -15.5f,  1.0f,0.0f,1.0f, 1.0f, 1.0f,
      1.5f,  1.5f,  15.5f,  1.0f,0.0f,1.0f, 1.0f, 0.0f,
      1.5f,  1.5f,  15.5f,  1.0f,0.0f,1.0f, 1.0f, 0.0f,
      -1.5f,  1.5f,  15.5f, 1.0f,0.0f,1.0f, 0.0f, 0.0f,
      -1.5f,  1.5f, -15.5f, 1.0f,0.0f,1.0f,  0.0f, 1.0f

    };


    unsigned int VBO1,VAO1,VBO2,VAO2;
    
    glGenVertexArrays(1,&VAO1);
    glGenBuffers(1,&VBO1);    
    glBindVertexArray(VAO1);    
    glBindBuffer(GL_ARRAY_BUFFER,VBO1);    
    glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);		
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);    
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
	
    glGenVertexArrays(1,&VAO2);
    glGenBuffers(1,&VBO2);    
    glBindVertexArray(VAO2);    
    glBindBuffer(GL_ARRAY_BUFFER,VBO2);    
    glBufferData(GL_ARRAY_BUFFER,sizeof(smallcubevertices),smallcubevertices,GL_STATIC_DRAW);		
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);    
    //    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    //    glEnableVertexAttribArray(1);

    
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 
	
    // uploading the image data of the texture 1
    unsigned char* data1 = stbi_load("brickwall.jpg",&width,&height,&nrChannels,0);
    if(data1){
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data1);
      glGenerateMipmap(GL_TEXTURE_2D);
    }else{
      std::cout<<"Could not load texture1 image"<<std::endl;
    }
    stbi_image_free(data1);//free the buffer
    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    // uploading the image data of the texture 2
    unsigned char* data2 = stbi_load("awesomeface.png",&width,&height,&nrChannels,0);
    if(data2){
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data2);
      glGenerateMipmap(GL_TEXTURE_2D);
    }else{
      std::cout<<"Could not load texture2 image"<<std::endl;
    }
    stbi_image_free(data2);// free the buffer
    
    
    //    ourShader.use();
 	
    while(!glfwWindowShouldClose(window)){
      
      processInput(window);

      glClearColor(0.2f,0.3f,0.3f,1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // bind textures on corresponding texture units
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);
  
      // activate shader
      ourShader.use();
      ourShader.setInt("texture1", 0);
      ourShader.setInt("texture2", 1);

      // create transformations
      glm::mat4 view(1.0f);
      glm::mat4 projection(1.0f);
      
      glm::mat4 roommodel(1.0f);
      glm::mat4 smallCubeModel(1.0f);
      
      
      view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -50.0));
      projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
      
      //      model = glm::rotate(model,(float)glfwGetTime()*glm::radians(20.0f),glm::vec3(1.0f,0.3f,0.5f));
      smallCubeModel = glm::rotate(smallCubeModel,(float)glfwGetTime()*glm::radians(20.0f),glm::vec3(1.0f,0.3f,0.5f));
	
	ourShader.setMat4("model",roommodel);
	ourShader.setMat4("view",view);
	ourShader.setMat4("projection",projection);
	
	ourShader.setFloat("mixValue",mixValue);
	
	//        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
	//	unsigned int projLoc  = glGetUniformLocation(ourShader.ID, "projection");

	
        // pass them to the shaders (3 different ways)
	//       glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        //glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(VAO1);
	glDrawArrays(GL_TRIANGLES,0,36);

	smallCubeShader.use();
	smallCubeShader.setMat4("model",smallCubeModel);
	smallCubeShader.setMat4("view",view);
	smallCubeShader.setMat4("projection",projection);
	
	glBindVertexArray(VAO2);
	glDrawArrays(GL_TRIANGLES,0,36);

        glfwSwapBuffers(window);
        glfwPollEvents();
	
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);

    glfwTerminate();
    return 0;

}

void processInput(GLFWwindow *window){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
    //      mixValue += 0.001f;
      farPlaneDistance += 0.2f;
      
      //if(mixValue >= 1.0f)
      //	mixValue = 1.0f;

      if(farPlaneDistance >= -0.5f)
	farPlaneDistance = -70.0f;
    }

    if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
      //      mixValue -= 0.001f;
      farPlaneDistance -= 0.2f;
      
      //      if(mixValue <= 0.0f)
      //	mixValue = 0.0f;

      if(farPlaneDistance <= -80.0f)
	farPlaneDistance = -70.0f;
      
    }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

