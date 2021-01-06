#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "shader.hpp"

class Renderer
{
 public:
  Renderer(){}

  //void setUniformValue(const Shader& sh, float v1, float v2, float v3, float v4);
  void draw(const VertexArray& vao, const IndexBuffer& ib, const Shader& sh);
  void drawArray(const VertexArray& vao, const Shader& sh, unsigned int vertexNum);

  void setShaderUniform(const std::string& str, const Shader& sh, float v1, float v2, float v3, float v4);

  void clearWindow();
};
