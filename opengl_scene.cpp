#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"shader.h"


void framebuffer_size_callback(GLFWwindow* window,int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;
float farPlaneDistance = -70.0f;

float rotationAngle;
bool rotate = true;

glm::vec3 camPos = glm::vec3(0.0f,0.0f,50.0f);
glm::vec3 camFront = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 camUp = glm::vec3(0.0f,1.0f,0.0f);


bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void setVerticesProp(unsigned int* VAO,unsigned int* VBO, float* vertexArray,unsigned int arraysize){  
  glGenVertexArrays(1,VAO);
  glGenBuffers(1,VBO);    
  glBindVertexArray(*VAO);    
  glBindBuffer(GL_ARRAY_BUFFER,*VBO);    
  glBufferData(GL_ARRAY_BUFFER,arraysize,vertexArray,GL_STATIC_DRAW);		
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
  glEnableVertexAttribArray(0);    
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
}


unsigned int createTexture(std::string filename)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
	
    // uploading the image data of the texture 1
    unsigned char* data1 = stbi_load(filename.c_str(),&width,&height,&nrChannels,0);
    if(data1){
      
      GLenum format;
      if (nrChannels == 1)
	format = GL_RED;
      else if (nrChannels == 3)
	format = GL_RGB;
      else if (nrChannels == 4)
	format = GL_RGBA;

      glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data1);
      glGenerateMipmap(GL_TEXTURE_2D);
    }else{
      std::cout<<"Could not load "<< filename <<" image"<<std::endl;
    }
    stbi_image_free(data1);//free the buffer
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
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
  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );

    Shader ourShader("shader_collection/coordinateSystem.vs", "shader_collection/coordinateSystem.fs");

    Shader smallCubeShader("shader_collection/coloredCubeObject.vs","shader_collection/coloredCubeObject.fs");

    float cubeVerticesback[] = {
      // back side
      -20.5f, -20.5f, -50.5f,  0.0f, 0.0f,
      20.5f, -20.5f, -50.5f,  1.0f, 0.0f,
      20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      -20.5f,  20.5f, -50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 0.0f,
    };
    float cubeVerticesfront[]={
      //front side
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      20.5f, -20.5f,  50.5f,  1.0f, 0.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 1.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 1.0f,
      -20.5f,  20.5f,  50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
    };
    float cubeVerticesleft[]={
      //left side
      -20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
      -20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      -20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
    };

    float cubeVerticesright[]={
      //right side
      20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
      20.5f,  20.5f, -50.5f,  1.0f, 1.0f,
      20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      20.5f,  20.5f,  50.5f,  1.0f, 0.0f,
    };

    float cubeVerticesbottom[]={
      //bottom side
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
      20.5f, -20.5f, -50.5f,  1.0f, 1.0f,
      20.5f, -20.5f,  50.5f,  1.0f, 0.0f,
      20.5f, -20.5f,  50.5f,  1.0f, 0.0f,
      -20.5f, -20.5f,  50.5f,  0.0f, 0.0f,
      -20.5f, -20.5f, -50.5f,  0.0f, 1.0f,
    };

    float cubeVerticestop[]={
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


    unsigned int VBO1,VAO1,VBO2,VAO2,VBO3,VAO3,VBO4,VAO4,VBO5,VAO5,VBO6,VAO6;
    unsigned int sVAO,sVBO;

    setVerticesProp(&VAO1,&VBO1,cubeVerticesfront,sizeof(cubeVerticesfront));
    setVerticesProp(&VAO2,&VBO2,cubeVerticesback,sizeof(cubeVerticesback));
    setVerticesProp(&VAO3,&VBO3,cubeVerticesright,sizeof(cubeVerticesright));
    setVerticesProp(&VAO4,&VBO4,cubeVerticesleft,sizeof(cubeVerticesleft));
    setVerticesProp(&VAO5,&VBO5,cubeVerticestop,sizeof(cubeVerticestop));
    setVerticesProp(&VAO6,&VBO6,cubeVerticesbottom,sizeof(cubeVerticesbottom));
    
    glGenVertexArrays(1,&sVAO);
    glGenBuffers(1,&sVBO);    
    glBindVertexArray(sVAO);    
    glBindBuffer(GL_ARRAY_BUFFER,sVBO);    
    glBufferData(GL_ARRAY_BUFFER,sizeof(smallcubevertices),smallcubevertices,GL_STATIC_DRAW);		
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);    

    stbi_set_flip_vertically_on_load(true); 

    unsigned int texture2 = createTexture("texture_collection/awesomeface.png");
    unsigned int texture1 = createTexture("texture_collection/brickwall.jpg");
    	
    while(!glfwWindowShouldClose(window)){

      float currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;
      
      processInput(window);

      glClearColor(0.2f,0.3f,0.3f,1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // bind textures on corresponding texture units

      // create transformations
      glm::mat4 view(1.0f);
      glm::mat4 projection(1.0f);
      
      glm::mat4 roommodel(1.0f);
      glm::mat4 smallCubeModel(1.0f);
      

      float radius = 10.0f;
      float camx = sin(glfwGetTime()) * radius;
      float camz = cos(glfwGetTime()) * radius;

      
      view  = glm::lookAt(camPos, camPos+camFront,camUp);
      projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
      
      //      model = glm::rotate(model,(float)glfwGetTime()*glm::radians(20.0f),glm::vec3(1.0f,0.3f,0.5f));
      smallCubeModel = glm::rotate(smallCubeModel,(float)glfwGetTime()*glm::radians(20.0f),glm::vec3(1.0f,0.3f,0.5f));
      ourShader.bind();
      
      ourShader.setMat4("model",roommodel);
      ourShader.setMat4("view",view);
      ourShader.setMat4("projection",projection);
      
      ourShader.setFloat("mixValue",mixValue);
      
      // activate shader
      ourShader.setInt("texture1", 0);      
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture1);      
      
      glBindVertexArray(VAO1);
      glDrawArrays(GL_TRIANGLES,0,6);
      
      glBindVertexArray(VAO2);
      glDrawArrays(GL_TRIANGLES,0,6);

      glBindVertexArray(VAO3);
      glDrawArrays(GL_TRIANGLES,0,6);

      glBindVertexArray(VAO4);
      glDrawArrays(GL_TRIANGLES,0,6);
      
      ourShader.setInt("texture1", 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture2);


      glBindVertexArray(VAO5);
      glDrawArrays(GL_TRIANGLES,0,6);

      glBindVertexArray(VAO6);
      glDrawArrays(GL_TRIANGLES,0,6);
      
      smallCubeShader.bind();
      smallCubeShader.setMat4("model",smallCubeModel);
      smallCubeShader.setMat4("view",view);
      smallCubeShader.setMat4("projection",projection);
      
      glBindVertexArray(sVAO);
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

    float camSpeed = 2.5 * deltaTime;
    
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
      camPos += camSpeed * camFront;

    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
      camPos -= camSpeed * camFront;

    if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
      // when you do cross product of front vector and up vector of camera, you get right and left vectors. That result vector will be used as
      // a position of the camera
      camPos -= glm::normalize(glm::cross(camFront,camUp)) * camSpeed;

    if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
      // when you do cross product of front vector and up vector of camera, you get right and left vectors. That result vector will be used as
      // a position of the camera
      camPos += glm::normalize(glm::cross(camFront,camUp)) * camSpeed;      

    if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS)
      camFront = -camFront;      
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

