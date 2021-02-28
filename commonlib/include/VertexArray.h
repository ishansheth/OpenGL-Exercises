#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
  unsigned int m_arrayID;
  
 public:
  VertexArray();
  ~VertexArray();

  void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& l);
  
  void bind() const;

  void unbind() const;
};
