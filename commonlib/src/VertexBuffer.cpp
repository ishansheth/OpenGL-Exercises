#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
  glGenBuffers(1,&m_rendererID);  
  glBindBuffer(GL_ARRAY_BUFFER,m_rendererID);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(const std::vector<Vertex>& data)
{
  glGenBuffers(1,&m_rendererID);  
  glBindBuffer(GL_ARRAY_BUFFER,m_rendererID);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex), &data[0], GL_STATIC_DRAW);
}


VertexBuffer::~VertexBuffer()
{
  glDeleteBuffers(1,&m_rendererID);
}

void VertexBuffer::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER,m_rendererID);
}

void VertexBuffer::unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
