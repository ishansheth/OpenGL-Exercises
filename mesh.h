#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "shader.hpp"


struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoord;
};


struct Texture
{
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh
{
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  
  Mesh(std::vector<Vertex> v, std::vector<unsigned int> i, std::vector<Texture> t)
  {
    vertices = v;
    indices = i;
    textures = t;

    setupMesh();
  }

  void Draw(Shader& s)
  {
    s.bind();
    
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for(unsigned int i = 0; i < textures.size(); i++)
      {
	glActiveTexture(GL_TEXTURE0 + i);
	std::string number;
	std::string name = textures[i].type;

	if(name == "texture_diffuse")
	  number = std::to_string(diffuseNr);
	else if(name == "texture_specular")
	  number = std::to_string(specularNr);
	else if(name == "texture_normal")
	  number = std::to_string(normalNr);
	else if(name == "texture_height")
	  number = std::to_string(heightNr);

	glUniform1i(glGetUniformLocation(s.ID, (name + number).c_str()), i);
	glBindTexture(GL_TEXTURE_2D, textures[i].id);
      }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
  }

private:
  unsigned int VAO, VBO, EBO;
  
  void setupMesh()
  {    
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,texcoord));

    /*
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,bitangent));
    */
    
    glBindVertexArray(0);
  }
  
};

