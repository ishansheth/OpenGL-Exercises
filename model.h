#include "mesh.h"
#include "shader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
{
  std::string filename = std::string(path);
  
  filename = directory + '/' + filename;
  
  unsigned int textureID;
  glGenTextures(1,&textureID);
  
  int width, height, nrComponents;
  
  
  unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  
  if(data)
    {
      GLenum format;
      if(nrComponents == 1)
	format = GL_RED;
      else if(nrComponents == 3)
	format = GL_RGB;
      else if(nrComponents == 4)
	format = GL_RGBA;
      
      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      
      stbi_image_free(data);
    }
  else
    {
      std::cout<<"Texture failed to load:"<<path<<std::endl;
      stbi_image_free(data);	
    }
  return textureID;
}

class Model
{  
  void loadModel(std::string path)
  {
    const aiScene *scene = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
      {
	std::cout<<"ERROR::ASSIMP::"<< importer.GetErrorString() << std::endl;
	return;
      }
    directory = path.substr(0,path.find_last_of("/"));
    processNode(scene->mRootNode,scene);
    
  }
  
  void processNode(aiNode* node, const aiScene* scene)
  {
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
      {
	aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
	meshes.push_back(processMesh(mesh,scene));
      }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
      {
	processNode(node->mChildren[i],scene);
      }
  }
  
  Mesh processMesh(aiMesh* mesh, const aiScene* scene)
  {
    if(isLog)std::cout<<"======================> processing mesh"<<std::endl;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
      {
	Vertex v;
	
	{

	  glm::vec3 vec;
	  
	  vec.x = mesh->mVertices[i].x;
	  vec.y = mesh->mVertices[i].y;
	  vec.z = mesh->mVertices[i].z;
	  v.position = vec;
	}
	
	if(mesh->HasNormals())
	  {

	    glm::vec3 vec;
	    vec.x = mesh->mNormals[i].x;
	    vec.y = mesh->mNormals[i].y;
	    vec.z = mesh->mNormals[i].z;
	    v.normal = vec;
	  }

	if(mesh->mTextureCoords[0])
	  {

	    glm::vec2 texvec;
	    texvec.x = mesh->mTextureCoords[0][i].x;
	    texvec.y = mesh->mTextureCoords[0][i].y;

	    v.texcoord = texvec;
	  }
	else
	  v.texcoord = glm::vec2(0.0f, 0.0f);
	
	vertices.push_back(v);
      }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
      {
	aiFace face = mesh->mFaces[i];
	for(unsigned int j = 0; j < face.mNumIndices; j++)
	    indices.push_back(face.mIndices[j]);
	
      }

    if(mesh->mMaterialIndex >= 0)
      {
	if(isLog)std::cout<<"............ processing materials"<<std::endl;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	if(isLog)std::cout<<"diffuse texture size:"<<diffuseMaps.size()<<std::endl;
	
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	if(isLog)std::cout<<"specular texture size:"<<diffuseMaps.size()<<std::endl;
	
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	if(isLog)std::cout<<"normal texture size:"<<diffuseMaps.size()<<std::endl;
	
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	if(isLog)std::cout<<"height texture size:"<<diffuseMaps.size()<<std::endl;
      }
    
    return Mesh(vertices, indices, textures);
  }

  
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string texTypename)
    {

      if(isLog)std::cout<<"............ loading material into texture"<<std::endl;
      
      std::vector<Texture> textures;
      
      for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
	  aiString str;
	  mat->GetTexture(type,i,&str);
	  bool skip = false;
	  for(unsigned int j = 0; j < textures_loaded.size(); j++)
	    {
	      if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
		{
		  textures.push_back(textures_loaded[j]);
		  skip = true;
		  break;
		}
	    }

	  if(!skip)
	    {
	      Texture texture;
	      texture.id = TextureFromFile(str.C_Str(), this->directory);
	      texture.type = texTypename;
	      texture.path = str.C_Str();
	      textures.push_back(texture);
	      textures_loaded.push_back(texture);
	    }
	}      
      return textures;      
    }
  
  
 private:
  Assimp::Importer importer;
    

  std::vector<Texture> textures_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
  bool gammaCorrection;

  bool isLog;
  
 public:

  std::size_t getNumMeshes() { return meshes.size();}

 Model(const char* path, bool logOn):
  isLog(logOn)
  {
    loadModel(path);
  }
  
  void Draw(Shader& s)
  {
    for(auto& mesh : meshes)
      mesh.Draw(s);
  }
    
};
