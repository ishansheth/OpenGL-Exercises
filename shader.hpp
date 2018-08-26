#ifndef SHADER_HPP
#define SHADER_HPP

#include<string>
#include<fstream>
#include<sstream>
#incldue<iostream>
include<GL/glew.h>


class Shader{

public:
  unsigned int ID;
  // constructor reads and builds shader
  Shader(const GLchar* vertexPath,const GLchar* fragmentPath);

  // use/activate shader
  void use();

  //utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string& name,int value);
  void setFloat(const std::string& name,float value);
};
#endif
