#include "VertexArray.h"

VertexArray::VertexArray()
{
  glGenVertexArrays(1,&m_arrayID);
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1,&m_arrayID);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
  bind();
  vb.bind();
  const auto& elements = layout.getElement();
  unsigned int offset = 0;
  for(unsigned int i = 0; i < elements.size(); i++)
  {
    const auto& ele = elements[i];
    glVertexAttribPointer(i, ele.count, ele.type, ele.normalized ? GL_TRUE: GL_FALSE, layout.getStride(), (void*)offset);
    glEnableVertexAttribArray(i);
    offset += ele.count*VertexBufferLayoutElement::getSizeOfType(ele.type);
  }
}

void VertexArray::bind() const
{
  glBindVertexArray(m_arrayID);
}

void VertexArray::unbind() const
{
  glBindVertexArray(0);
}
