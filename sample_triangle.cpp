#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<cmath>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width,int height);
void processInput(GLFWwindow* window,glm::mat4&);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/**
   The vertex shader which takes vertex co-ordinates and color of the triangle shape as input, indicated by different location 
   Output of the vertex color is vertexColor variable of type vec3
 **/

const char* vertexWithColorShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 vertexColor;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "       vertexColor = aColor;\n"
	"}\0";

/**
   The vertex shader which takes vertex co-ordinates,color and texture co-ordinates of 
   the triangle shape as input, indicated by different location 
   Output of the vertex color is vertexColor variable of type vec3, Texture co-ordinates of type vec2
 **/

const char* vertexWithColorTextureShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aTex;\n"

        "out vec3 ourColor;\n"
        "out vec2 TexCoord;\n"

        "void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "       ourColor = aColor;\n"
        "       TexCoord = vec2(aTex.x,aTex.y);\n"
	"}\0";


/**
   In this fragment color, output is fragment color which is set to uniform variable ourColor. This
   uniform variable can be set from outside
 **/
const char* changeColorFragment  = "#version 330 core\n"
	"out vec4 FragColor;\n"
        "uniform vec4 ourColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = ourColor;\n"
	"}\0";


/**
   fragment shader which takes vertexColor as input from vertex shader and sets it to output variable FragColor
 **/
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
        "in vec3 vertexColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(vertexColor,1.0);\n"
	"}\0";


/**
   fragment shader which takes Texture coordinates as input from vertex shader and output is fragment color
   It has a uniform variable 'mixvalue' of type float which determines the amout of visibility of second texture.
   Two other uniform variables of type sampler2D are used to represent two textures which can be activated from outside
 **/

const char* fragmentWithTextureShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
  
        "in vec2 TexCoord;\n"

        "uniform float mixvalue;\n"
  
        "uniform sampler2D texture1;\n"
        "uniform sampler2D texture2;\n"
	"void main()\n"
	"{\n"
	"	FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),mixvalue);\n"
	"}\0";


/**
   Vertex shader which takes vertex co-ordinates,color and texture co-ordinates as input and texture co-ordinates as
   output. The shader has transformation matrix as uniform which is used to determine the vertices. and transformation matrix can
   be set from outside for the orientation
 **/
const char* rotationVertexShader = "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "layout (location = 1) in vec3 aColor;\n"
  "layout (location = 2) in vec2 aTexCoord;\n"

  "out vec2 TexCoord;\n"

  "uniform mat4 transform;\n"

  "void main()\n"
  "{\n"
  "        gl_Position = transform * vec4(aPos,1.0f);\n"
  "        TexCoord = vec2(aTexCoord.x,aTexCoord.y);\n"
  "}\0";

  
float mixvalue = 0.2;

int main(){  

	// initializing GLFW library APIs
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
	// creating GLFW window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"LearnOpenGL",NULL,NULL);

	// check if the window is NULL
	if(window == NULL){
		std::cout<<"Failed to create window"<<std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	
	// build and compile the shader programm
	//vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader,1,&rotationVertexShader,NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
		std::cout<<"Error in the vertex shader compilation\n"<<infoLog<<std::endl;
	}
	
	//fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader,1,&fragmentWithTextureShaderSource,NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
		std::cout<<"Error in the fragment shader compilation\n"<<infoLog<<std::endl;
	}
	
	// link vertex and fragment shader	
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	glLinkProgram(shaderProgram);
	
	glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(shaderProgram,512,NULL,infoLog);
		std::cout<<"Error in theshader linking\n"<<infoLog<<std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/**
	   here, the coordinates of the vertices and corresponding texture coordinates are important and it should match with the texture parameters 
	   and fragment shader to achieve what you want.

	 **/

	// four vertices of the square;
	float vertices[] = {
	  // positions                //colors        //texture
	  1.0f,1.0f,0.0f,          1.0f,0.0f,0.0f,    2.0f,2.0f,
	  1.0f,-1.0f,0.0f,         0.0f,1.0f,1.0f,    2.0f,0.0f,
	  -1.0f,-1.0f,0.0f,        0.0f,0.0f,1.0f,    0.0f,0.0f,
	  -1.0f,1.0f,0.0f,	   1.0f,1.0f,1.0f,    0.0f,2.0f
	};
	
	unsigned int indices[] = {
	  0,1,3,
	  1,2,3
	};
       	
	unsigned int VBO,EBO,VAO;
	
	// bind the vertex array object first, then bind and set vertex buffers, then configure vertex attributes
	glGenVertexArrays(1,&VAO);
	
	// create 2 buffers , one for vertex buffer object and other element buffer object	
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
	
	// bind vertex array first
	glBindVertexArray(VAO);
	
	// bind vertex buffer and fill data into that
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

	// bind element buffer and fill data of indices in that
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
	
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);	
	
	//	glBindBuffer(GL_ARRAY_BUFFER,0); // optional
	//	glBindVertexArray(0); // optional


	unsigned int texture1,texture2;
	int width,height,nrChannels;

	stbi_set_flip_vertically_on_load(true);
	glGenTextures(1,&texture1);
	glBindTexture(GL_TEXTURE_2D,texture1);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	
	unsigned char* data1 = stbi_load("brickwall.jpg",&width,&height,&nrChannels,0);
	if(data1){
	  	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
	  std::cout<<"Could not load texture1 image"<<std::endl;
	}
	stbi_image_free(data1);

	glGenTextures(1,&texture2);
	glBindTexture(GL_TEXTURE_2D,texture2);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	

	unsigned char* data2 = stbi_load("awesomeface.png",&width,&height,&nrChannels,0);
	if(data2){
	  	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
	  std::cout<<"Could not load texture2 image"<<std::endl;
	}
	stbi_image_free(data2);

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram,"texture1"),0);
	glUniform1i(glGetUniformLocation(shaderProgram,"texture2"),1);
	

	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window)){
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texture1);
	        glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texture2);


		glm::mat4 trans(1.0f);

		processInput(window,trans);

		glUseProgram(shaderProgram);
		glUniform1f(glGetUniformLocation(shaderProgram,"mixvalue"),mixvalue);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"transform"),1,GL_FALSE,glm::value_ptr(trans));

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram,"ourColor");
		if(vertexColorLocation != -1){
  		  glUniform4f(vertexColorLocation,0.0f,greenValue,0.0f,1.0f);
		}

     		glBindVertexArray(VAO);

		
//		if you uncomment the below line and comment glDrawElements, then only first 3 vertices 
//		will be taken and triagle will be drawn in the wireframe mode
//		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
    	glDeleteBuffers(1, &EBO);
			
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window,glm::mat4& trans){
	if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window,true);

	if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS){
	  mixvalue += 0.001f;
	  if(mixvalue >= 1.0f)
	    mixvalue = 1.0f;
	}

	if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS){
	  mixvalue -= 0.001f;
	  if(mixvalue <= 0.0f)
	    mixvalue = 0.0f;
	}

	if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS){
	  trans = glm::rotate(trans,(float)glfwGetTime(),glm::vec3(0.0,0.0,1.0));
	  trans = glm::scale(trans,glm::vec3(0.5f,0.5f,0.5f));
	}

}

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
	glViewport(0,0,width,height);
}




