/**
g++ -o perspectiveRendering <capture_cam_render.cpp> stb_image.h shader.hpp  `pkg-config --static --libs glfw3` -lGL -lGLEW
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

#include "opencv2/opencv.hpp"
#include <vector>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// here image from camera is in JPEG format stored, but one also try PNG
const std::string imageFileName = "capture.jpg";
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
unsigned int texture1, texture2;
float mixValue = 0.2f;
float farPlaneDistance = -70.0f;


void* grab_frame(){

  using namespace cv;

  VideoCapture cap;
  if(!cap.open(0)){
    std::cout<<"Video device is not yet open"<<std::endl;
    return 0;
  }

  Mat frame(480,640,CV_8UC4);
  
  cap >> frame;
  try{
    imwrite(imageFileName,frame);
  }catch(std::runtime_error& ex){
    std::cout<<"Could not write image data to png file"<<std::endl;
    return 0;
  }  
}


void createTexture(){

    grab_frame();

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

    unsigned char* data2 = stbi_load(imageFileName.c_str(),&width,&height,&nrChannels,0);
    
    if(data2){
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data2);
      glGenerateMipmap(GL_TEXTURE_2D);
    }else{
      std::cout<<"Could not load texture2 image"<<std::endl;
    }
    stbi_image_free(data2);// free the buffer
  
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
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

    Shader ourShader("camCapture.vs", "camCapture.fs");

    
    float vertices[] = {
        // positions          // texture coords
         0.0f,  0.5f, 0.5f,   1.0f, 1.0f, // top right
         0.0f, 0.5f, -0.5f,   1.0f, 0.0f, // bottom right
         0.0f, -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
         0.0f,  -0.5f, 0.5f,   0.0f, 1.0f  // top left 
    };

    float cubeVertices[] = {
      // back side
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      //front side
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      //left side
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      //right side
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      //bottom side
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      //top side
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
	//        glActiveTexture(GL_TEXTURE1);
	//        glBindTexture(GL_TEXTURE_2D, texture2);
  
        // activate shader
        ourShader.use();
      
        // create transformations
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
	glm::mat4 model(1.0f);

	// try rotaing here with respect to different axis, try simple square and cube, 

	// if you do not translate the cube to -3.0 in z axis and rotate it, you will see yourself inside the cube.
	//Its FUN!!
	//        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, farPlaneDistance));
	
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f,-10.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  	
	model = glm::rotate(model,(float)glfwGetTime()*glm::radians(20.0f),glm::vec3(1.0f,0.3f,0.5f));

	ourShader.setMat4("model",model);	
	ourShader.setFloat("mixValue",mixValue);
	
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
	unsigned int projLoc  = glGetUniformLocation(ourShader.ID, "projection");

	
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,36);

	//	        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
      mixValue += 0.001f;
      farPlaneDistance += 0.2f;
      
      if(mixValue >= 1.0f)
	mixValue = 1.0f;

      if(farPlaneDistance >= -3.0f)
	farPlaneDistance = -70.0f;
    }

    if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
      mixValue -= 0.001f;
      farPlaneDistance -= 0.2f;
      
      if(mixValue <= 0.0f)
	mixValue = 0.0f;

      if(farPlaneDistance <= -80.0f)
	farPlaneDistance = -70.0f;
      
    }

    if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS){
      createTexture();
      //      glActiveTexture(GL_TEXTURE0);
      //      glBindTexture(GL_TEXTURE_2D, texture1);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);

    }    
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

