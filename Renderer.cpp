#include "Renderer.h"

void Renderer::draw(const VertexArray& vao, const IndexBuffer& ib, const Shader& sh)
{
  vao.bind();
  ib.bind();
  sh.bind();

  glDrawElements(GL_TRIANGLES,ib.getCount(),GL_UNSIGNED_INT, 0);

}

void Renderer::drawArray(const VertexArray& vao, const Shader& sh, unsigned int vertexNum)
{
  vao.bind();
  sh.bind();
  glDrawArrays(GL_TRIANGLES, 0, vertexNum);
}

void Renderer::setShaderUniform(const std::string& str, const Shader& sh, float v1, float v2, float v3, float v4)
{
  sh.bind();
  sh.setVec4(str, v1, v2, v3, v4);
}

void Renderer::clearWindow()
{
  glClear(GL_COLOR_BUFFER_BIT);
}
