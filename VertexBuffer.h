#pragma once

#include <GL/glew.h>

class VertexBuffer
{
  unsigned int m_rendererID;
  
 public:
  VertexBuffer(const void* data, unsigned int size);
  ~VertexBuffer();

  void bind() const;
  void unbind() const;
};
