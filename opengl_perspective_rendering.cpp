#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"shader.hpp"

/**
This tutorial demonstrates the co-ordinate systems in openGL rendering process.
There are total 5 co-ordinate systems used in openGL. All the obejcts go through the transformation according to these
co-ordinate systems because some of the operations on the object is easier in particular co-ordinate system than other.
After the different transformation of the co-ordinate of the object, they are finally rendered if they are visible to the viewer/camera
or those co-ordinates are discarded. 

1.Local space: local space is the co-ordinate space that is local to your object. 

2.World space: If all the objects are places togather, they will end up at different position relative to each-other with respect
to the origin of the world. If the cube is at (0,0,0), it will end up at different location when its place in the world co-ordinate system 

3.View space: The view space is what usually people refer as a camera of OpenGL. The view space is the result of transforming your world-space
co-ordinate to coordinates that are in front of user's view.

4.Clip space: At the end of each vertex shader run, OpenGl expects the vertices to be in the specific range and any coordinate that fall outside 
is clipped. The coordinates that are clipped are discarded, so the remaining coordinates  will end up as fragment visible on oyur screen. 
To transform vertex coordinates from view to clip space, we define a projection matrix that specifies  a range of co-ordinates e.g. -1000 to 1000. 
The projection matrix then transforms them to NDC(Normalised device co-ordinates -1 to 1)  So the co-ordinate (1250,50,750) will be discarded 
becausex x axis is outside the range. This viewing box projection matrix is called a frustrum and each coordinate that ends up inside this frustrum
will end up on the user screen. The total process to convert coordinates within a specified range to NDC that can be easily be mapped to 2D view 
space is called projection since the projection matrix projects 3D coordinates to the easy to map 2D normalized device coordinates. 

**/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;
float farPlaneDistance = -70.0f;

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

    /**
       openGL stores all of its depth information in a z-buffer, which is also known as depth buffer. The depth is stored within each fragment and whenever fragment wants to output its color, openGL compares its depth values with the z-buffer and if current fragment is behind the other fragment, it is discarded, otherwise overwritten. This process is called depth testing. For this, we have to tell openGL to perform depth testing. We can do that by 'glEnable(GL_DEPTH_TEST)'. In this, we want depth test to be enabled for the cube rendering. So we use that.
     **/
    glEnable(GL_DEPTH_TEST);
    
    if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      getchar();
      glfwTerminate();
      return -1;
    }

    Shader ourShader("coordinateSystem.vs", "coordinateSystem.fs");
1
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
    

    /**
       when you draw a cube, and use EBO, you will see some weird texture on each side  and not cube with texture. 
       Therefore, when we draw cube, we use VAO and not EBO. The reason for that is: With EBO, you can select which
       vertices you want to draw to create the shape. But each vertex should be unique. That means, the combination of
       all its vertex attributes should be unique. (in this case position coordinates and texture coordinates)

       In the case of cube, the top right vertex of a face has a different texture coordinate then the same vertex of 
       neighbour face as the texture is differently oriented there. In that case, you can not simply use that single 
       vertex as the cube's corner as its not unique for all the surrounding faces.        
     **/
    
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
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


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


    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
  
        // activate shader
        ourShader.use();
      
        // create transformations
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);

	// try rotaing here with respect to different axis, try simple square and cube, 

	// if you do not translate the cube to -3.0 in z axis and rotate it, you will see yourself inside the cube.
	//Its FUN!!
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, farPlaneDistance));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	
        // retrieve the matrix uniform locations
	for(int i = 0;i<10;i++){
	  glm::mat4 model(1.0f);
	  	
	  model = glm::translate(model, cubePosition[i]);
	  float angle = 20.0f * (i+1);
	  model = glm::rotate(model,(float)glfwGetTime()*glm::radians(angle),glm::vec3(1.0f,0.3f,0.5f));
	  ourShader.setMat4("model",model);

	  glDrawArrays(GL_TRIANGLES,0,36);
	  
	}
	ourShader.setFloat("mixValue",mixValue);
	
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
	unsigned int projLoc  = glGetUniformLocation(ourShader.ID, "projection");

	
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(VAO);

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
    
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

