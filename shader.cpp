#include "shader.hpp"

Shader::Shader(const GLchar* vertexPath,const GLchar* fragmentPath){

  // retrieve the source code from the file
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  // ensure ifstream objects can throw exceptions
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try{
    // open files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream,fShaderStream;
    // read file's buffer content
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  }
  catch(std::ifstream::failure e)
  {
    std::cout<<"Shader file is not successfully loaded"<<std::endl;
  }
  
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  GLint Result = GL_FALSE;
  int InfoLogLength;

  printf("Compiling shader:%s\n",vertexPath);
  char const* VertexSourcePointer = vertexCode.c_str();
  glShaderSource(VertexShaderID,1,&VertexSourcePointer,NULL);

  glCompileShader(VertexShaderID);

  glGetShaderiv(VertexShaderID,GL_COMPILE_STATUS,&Result);
  glGetShaderiv(VertexShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);

  if(InfoLogLength > 0){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID,InfoLogLength,NULL,&VertexShaderErrorMessage[0]);
    printf("%s",&VertexShaderErrorMessage[0]);
  }


  printf("Compiling shader:%s\n",fragmentPath);
  char const* FragmentSourcePointer = fragmentCode.c_str();
  glShaderSource(FragmentShaderID,1,&FragmentSourcePointer,NULL);

  glCompileShader(FragmentShaderID);

  glGetShaderiv(FragmentShaderID,GL_COMPILE_STATUS,&Result);
  glGetShaderiv(FragmentShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);

  if(InfoLogLength > 0){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID,InfoLogLength,NULL,&FragmentShaderErrorMessage[0]);
    printf("%s",&FragmentShaderErrorMessage[0]);
  }

  printf("Linking Program\n");
  ID = glCreateProgram();
  glAttachShader(ID,VertexShaderID);
  glAttachShader(ID,FragmentShaderID);

  glLinkProgram(ID);

  glGetProgramiv(ID,GL_LINK_STATUS,&Result);
  glGetProgramiv(ID,GL_INFO_LOG_LENGTH,&InfoLogLength);
  if(InfoLogLength > 0){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(ID,InfoLogLength,NULL,&ProgramErrorMessage[0]);
    printf("%s",&ProgramErrorMessage[0]);
  }

  //  glDetachShader(ProgramID,VertexShaderID);
  //  glDetachShader(ProgramID,FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

}

void Shader::use(){
  glUseProgram(ID);
}

void Shader::setBool(const std::string& name,bool value)const{
  glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
}

void Shader::setInt(const std::string& name,int value){
  glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setFloat(const std::string& name,float value){
  glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setMat4(const std::string& name,const glm::mat4& mat){
  glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,&mat[0][0]);
}
