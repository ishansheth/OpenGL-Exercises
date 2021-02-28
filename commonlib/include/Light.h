#include <map>

enum class LightType
{
  Directional,
  Point,
  Spot,
  None
};


enum class LightParameters
{
  Position,
  Direction,
  Ambient,
  Diffuse,
  Specular,
  Color,
  Constant,
  Linear,
  Quadratic,
  Cutoff,
  Outercutoff
};


class Light
{
  int n_dirLight;
  int n_pointLight;
  int n_spotLight;
  Shader* shaderObj;
  
  
  LightType type;

  static std::string getPropertyName(LightParameters p)
  {
    static std::map<LightParameters,std::string> propertyNameMap;
    propertyNameMap[LightParameters::Position] = "position";
    propertyNameMap[LightParameters::Direction] = "direction";
    propertyNameMap[LightParameters::Ambient] = "ambient";
    propertyNameMap[LightParameters::Diffuse] = "diffuse";
    propertyNameMap[LightParameters::Specular] = "specular";
    propertyNameMap[LightParameters::Color] = "LightColor";
    propertyNameMap[LightParameters::Constant] = "constant";
    propertyNameMap[LightParameters::Linear] = "linear";
    propertyNameMap[LightParameters::Quadratic] = "quadratic";
    propertyNameMap[LightParameters::Cutoff] = "cutoff";
    propertyNameMap[LightParameters::Outercutoff] = "outercutoff";

    return propertyNameMap[p];
  }
  
  bool setDirectionalLightPara(unsigned int index, LightParameters p, const glm::vec3& value)
  {
    if(n_dirLight <= 1)
      {
	std::string uniformName = "dirLight." + getPropertyName(p);
	shaderObj->setVec3(uniformName,value);
      }
    else
      {
	std::string uniformName = "dirLight[" + std::to_string(index) + "]." + getPropertyName(p);
	shaderObj->setVec3(uniformName,value);
      }
  }

  bool setPointLightPara(unsigned int index, LightParameters p, const glm::vec3& value)
  {
    if(n_pointLight <= 1)
      {
	std::string uniformName = "pointLights." + getPropertyName(p);
	shaderObj->setVec3(uniformName,value);
      }
    else
      {
	std::string uniformName = "pointLights[" + std::to_string(index) + "]." + getPropertyName(p);
	shaderObj->setVec3(uniformName,value);
      }
  }

  bool setSpotLightPara(unsigned int index,LightParameters p, const glm::vec3& value)
  {
    if(n_spotLight <= 1)
      {
	std::string uniformName = "spotLight." + getPropertyName(p);
	shaderObj->setVec3(uniformName,value);
      }
    else
      {
	std::string uniformName = "spotLight[" + std::to_string(index) + "]." + getPropertyName(p);
	shaderObj->setVec3(uniformName,value);
      }
  }

  bool setDirectionalLightPara(unsigned int index, LightParameters p, const float& value)
  {
    if(n_dirLight <= 1)
      {
	std::string uniformName = "dirLight." + getPropertyName(p);
	shaderObj->setFloat(uniformName,value);
      }
    else
      {
	std::string uniformName = "dirLight[" + std::to_string(index) + "]." + getPropertyName(p);
	shaderObj->setFloat(uniformName,value);
      }
  }

  bool setPointLightPara(unsigned int index, LightParameters p, const float& value)
  {
    if(n_pointLight <= 1)
      {
	std::string uniformName = "pointLights." + getPropertyName(p);
	shaderObj->setFloat(uniformName,value);
      }
    else
      {
	std::string uniformName = "pointLights[" + std::to_string(index) + "]." + getPropertyName(p);
	shaderObj->setFloat(uniformName,value);
      }
  }

  bool setSpotLightPara(unsigned int index, LightParameters p, const float& value)
  {
    if(n_spotLight <= 1)
      {
	std::string uniformName = "spotLight." + getPropertyName(p);
	shaderObj->setFloat(uniformName,value);
      }
    else
      {
	std::string uniformName = "spotLight[" + std::to_string(index) + "]." + getPropertyName(p);
	shaderObj->setFloat(uniformName,value);
      }
  }
  
 public:
  
  Light(Shader* sh):type(LightType::None),shaderObj(sh)
  {
    //    shaderObj->bind();
  }
  
  void setLight(LightType t)
  {
    type = t;
  }

  void setLightNumber(int num)
  {
    if(type == LightType::None)
      return;
    
    if(type == LightType::Directional)
      n_dirLight = num;
    else if(type == LightType::Point)
      n_pointLight = num;
    else if(type == LightType::Spot)
      n_spotLight = num;
    
  }
  
  bool setParameter(LightParameters p, const glm::vec3& value)
  {
    if(type == LightType::Directional)
      {
	for(unsigned int i = 0;i<n_dirLight;i++)
	  setDirectionalLightPara(i,p,value);
      }
    else if(type == LightType::Point)
      {
	for(unsigned int i = 0;i<n_pointLight;i++)
	  setPointLightPara(i,p,value);
      }
    else if(type == LightType::Spot)
      {
	for(unsigned int i = 0;i<n_spotLight;i++)
	  setSpotLightPara(i,p,value);
      }
    return true;
  }
  
  bool setParameter(LightParameters p, const glm::vec3* value)
  {
    if(type == LightType::Directional)
      {
	for(unsigned int i = 0;i<n_dirLight;i++)
	  setDirectionalLightPara(i,p,value[i]);
      }
    else if(type == LightType::Point)
      {
	for(unsigned int i = 0;i<n_pointLight;i++)
	  setPointLightPara(i,p,value[i]);
      }
    else if(type == LightType::Spot)
      {
	for(unsigned int i = 0;i<n_spotLight;i++)
	  setSpotLightPara(i,p,value[i]);
      }

    return true;

  }
  
  bool setParameter(LightParameters p, float value)
  {
    if(type == LightType::Directional)
      {
	for(unsigned int i = 0;i<n_dirLight;i++)
	  setDirectionalLightPara(i,p,value);
      }
    else if(type == LightType::Point)
      {
	for(unsigned int i = 0;i<n_pointLight;i++)
	  setPointLightPara(i,p,value);
      }
    else if(type == LightType::Spot)
      {
	for(unsigned int i = 0;i<n_spotLight;i++)
	  setSpotLightPara(i,p,value);
      }
    
    return true;    
  }


  bool setParameter(LightParameters p, const float* value)
  {
    if(type == LightType::Directional)
      {
	for(unsigned int i = 0;i<n_dirLight;i++)
	  setDirectionalLightPara(i,p,value[i]);
      }
    else if(type == LightType::Point)
      {
	for(unsigned int i = 0;i<n_pointLight;i++)
	  setPointLightPara(i,p,value[i]);
      }
    else if(type == LightType::Spot)
      {
	for(unsigned int i = 0;i<n_spotLight;i++)
	  setSpotLightPara(i,p,value[i]);
      }

    return true;

  }

  
  void enable()
  {

  }

};
