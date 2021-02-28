#pragma once
#include <GL/glew.h>
#include <string>

class Texture
{
  unsigned int m_textureID;
  std::string m_filepath;
  unsigned char* m_buffer;
  int m_width, m_height, m_bpp;
  
 public:
  Texture(const std::string& filepath);
  ~Texture();

  void bind(unsigned int slot = 0) const;
  void unbind() const;
};
