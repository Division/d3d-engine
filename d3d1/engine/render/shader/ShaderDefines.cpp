#include "ShaderDefines.h"

const std::map<VertexAttrib, std::string> SHADER_ATTRIB_DEFINES = {
  { VertexAttrib::Position, "ATTRIB_POSITION" },
  { VertexAttrib::Normal, "ATTRIB_NORMAL" },
  { VertexAttrib::Tangent, "ATTRIB_TANGENT" },
  { VertexAttrib::Bitangent, "ATTRIB_BITANGENT" },
  { VertexAttrib::TexCoord0, "ATTRIB_TEXCOORD0" },
  { VertexAttrib::Corner, "ATTRIB_CORNER" },
  { VertexAttrib::VertexColor, "ATTRIB_VERTEX_COLOR" },
  { VertexAttrib::JointWeights, "ATTRIB_JOINT_WEIGHT" },
  { VertexAttrib::JointIndices, "ATTRIB_JOINT_INDEX" }
};


const std::map<ShaderResourceName, std::string> SHADER_RESOURCE_DEFINES = {
  { ShaderResourceName::Texture0, "RESOURCE_TEXTURE0" },
  { ShaderResourceName::Texture1, "RESOURCE_TEXTURE1" },
  { ShaderResourceName::NormalMap, "RESOURCE_NORMAL_MAP" },
  { ShaderResourceName::SpecularMap, "RESOURCE_SPECULAR_MAP" },
  { ShaderResourceName::ShadowMap, "RESOURCE_SHADOW_MAP" },
  { ShaderResourceName::ProjectorTexture, "RESOURCE_PROJECTOR_TEXTURE" },
  { ShaderResourceName::LightGrid, "RESOURCE_LIGHT_GRID" },
  { ShaderResourceName::LightIndices, "RESOURCE_LIGHT_INDICES" }
};

const std::map<ConstantBufferName, std::string> CONSTANT_BUFFER_DEFINES = {
  { ConstantBufferName::ObjectParams, "CONSTANT_BUFFER_OBJECT_PARAMS" },
  { ConstantBufferName::SkinningMatrices, "CONSTANT_BUFFER_SKINNING_MATRICES" },
  { ConstantBufferName::Light, "CONSTANT_BUFFER_LIGHT" },
  { ConstantBufferName::Camera, "CONSTANT_BUFFER_CAMERA" },
  { ConstantBufferName::Projector, "CONSTANT_BUFFER_PROJECTOR" }
};

const std::map<ShaderCaps, std::string> SHADER_CAPS_DEFINES = {
  { ShaderCaps::Color, "CAP_COLOR" },
  { ShaderCaps::ObjectData, "CAP_OBJECT_DATA" },
  { ShaderCaps::VertexColor, "CAP_VERTEX_COLOR" },
  { ShaderCaps::PointSize, "CAP_POINT_SIZE" },
  { ShaderCaps::Billboard, "CAP_BILLBOARD" },
  { ShaderCaps::Lighting, "CAP_LIGHTING" },
  { ShaderCaps::Texture0, "CAP_TEXTURE0" },
  { ShaderCaps::Texture1, "CAP_TEXTURE1" },
  { ShaderCaps::NormalMap, "CAP_NORMAL_MAP" },
  { ShaderCaps::SpecularMap, "CAP_SPECULAR_MAP" },
  { ShaderCaps::ProjectedTexture, "CAP_PROJECTED_TEXTURE" },
  { ShaderCaps::Skinning, "CAP_SKINNING" }
};

const std::string SHADER_IS_VERTEX = "IS_VERTEX";
const std::string SHADER_IS_PIXEL = "IS_PIXEL";
