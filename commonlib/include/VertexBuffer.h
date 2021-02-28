#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
  glm::vec3 position;
  bool isPos = false;
  glm::vec3 normal;
  bool isNormal = false;
  glm::vec2 texcoord;
  bool isTex = false;
};


class VertexBuffer
{
  unsigned int m_rendererID;
  
 public:
  VertexBuffer(const void* data, unsigned int size);
  VertexBuffer(const std::vector<Vertex>& data);
  
  ~VertexBuffer();

  void bind() const;
  void unbind() const;
};
