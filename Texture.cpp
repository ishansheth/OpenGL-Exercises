#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string& filepath): m_textureID(0),m_filepath(filepath),m_buffer(nullptr),
					       m_width(0),m_height(0),m_bpp(0)
{
  stbi_set_flip_vertically_on_load(1);
  m_buffer = stbi_load(m_filepath.c_str(), &m_width, &m_height, &m_bpp, 4);
  
  glGenTextures(1,&m_textureID);
  glBindTexture(GL_TEXTURE_2D,m_textureID);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

  if(m_buffer)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
  else
    std::cout<<"Failed to load texture"<<std::endl;
    
  glBindTexture(GL_TEXTURE_2D,0);
  
}

Texture::~Texture()
{
  glDeleteTextures(1,&m_textureID);
}

void Texture::bind(unsigned int slot) const
{
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D,m_textureID);
}

void Texture::unbind() const
{
  glBindTexture(GL_TEXTURE_2D,0);
}
