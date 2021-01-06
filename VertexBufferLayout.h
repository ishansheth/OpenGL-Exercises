#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferLayoutElement
{
  unsigned int count;
  unsigned int type;
  bool normalized;

  static unsigned int getSizeOfType(unsigned int type)
  {
    switch(type)
      {
      case GL_FLOAT : return 4;
      case GL_UNSIGNED_INT : return 4;
      case GL_UNSIGNED_BYTE : return 1;
      }
    return 0;
  }
};

template<typename T>
struct identity{};

class VertexBufferLayout
{
  std::vector<VertexBufferLayoutElement> m_elements;
  unsigned int m_stride;

  void Push(unsigned int count, identity<float>)
  {
    m_elements.push_back({count,GL_FLOAT,false});
    m_stride += (count*VertexBufferLayoutElement::getSizeOfType(GL_FLOAT));	
  }
  
  void Push(unsigned int count, identity<unsigned int>)
  {
    m_elements.push_back({count,GL_UNSIGNED_INT,false});
    m_stride += (count*VertexBufferLayoutElement::getSizeOfType(GL_UNSIGNED_INT));	
  }

  
 public:
 VertexBufferLayout():m_stride(0)
    {}
  
  ~VertexBufferLayout()
    {}

  template<typename T>
    void Push(unsigned int count)
    {
      Push(count, identity<T>{});
    }


  const std::vector<VertexBufferLayoutElement>& getElement() const {return m_elements;}
  unsigned int getStride() const { return m_stride;}
};
