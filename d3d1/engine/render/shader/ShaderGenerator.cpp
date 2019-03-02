//
// Created by Sidorenko Nikita on 3/23/18.
//

#include "ShaderGenerator.h"
#include "system/Logging.h"
#include <unordered_map>
#include <string>
#include "loader/ShaderLoader.h"
#include "Engine.h"
#include "Shader.h"


const std::string TEMPLATE_ROOT = "resources/shaders/";

const std::string TEMPLATE_LIST[] = {
    "root" // First goes the root template
};
/*    "camera",
    "object_params",
    "vertex_color_fragment_main",
    "vertex_color_fragment",
    "vertex_color_vertex_main",
    "vertex_color_vertex",
    "lighting_fragment_main",
    "lighting_fragment",
    "lighting_vertex_main",
    "lighting_vertex",
    "lighting_process_point",
    "lighting_process_spot",
    "lighting_process_projector",
    "lighting_process_decal",
    "normal_mapping_fragment_main",
    "normal_mapping_fragment",
    "normal_mapping_vertex_main",
    "normal_mapping_vertex",
    "skinning_vertex_main",
    "skinning_vertex",
    "texture_fragment_main",
    "texture_fragment",
    "terrain" */

const std::map<ShaderCaps, std::string> CAPS_TO_PARAM_MAP = {
    { ShaderCaps::Color, "COLOR" },
    { ShaderCaps::VertexColor, "VERTEX_COLOR" },
    { ShaderCaps::PointSize, "POINT_SIZE" },
    { ShaderCaps::Billboard, "BILLBOARD" },
    { ShaderCaps::Lighting, "LIGHTING" },
    { ShaderCaps::NormalMap, "NORMAL_MAP" },
    { ShaderCaps::SpecularMap, "SPECULAR_MAP" },
    { ShaderCaps::Texture0, "TEXTURE0" },
    { ShaderCaps::ProjectedTexture, "PROJECTED_TEXTURE" },
    { ShaderCaps::Skinning, "SKINNING" },
};

const auto ROOT_TEMPLATE = TEMPLATE_LIST[0];

ShaderGenerator::ShaderGenerator() {

}

void ShaderGenerator::_addTemplateCallback(std::string tplName) {
  auto env = _env;
  const auto &templateMap = _templateMap;
  auto name = tplName;
  _env.add_callback(tplName, 1, [&env, name, templateMap](inja::Parsed::Arguments args, json data) {
    auto param = env.get_argument<std::string>(args, 0, data);
    data["MODE"] = param;
    std::string res = env.render_template(templateMap.at(name), data);
    return res;
  });
}

std::string ShaderGenerator::generateShaderSource(ShaderCapsSetPtr caps, const std::string rootTemplate) const {
  json data = this->_getJSONForCaps(caps);
  //data["USE_BUFFER_TEXTURE"] = ENGINE_USE_BUFFER_TEXTURE;

  auto result = _env.render_template(_templateMap.at(rootTemplate), data);

  return result;
}

void ShaderGenerator::setupTemplates () {
  for (auto &filename : TEMPLATE_LIST) {
    auto path = TEMPLATE_ROOT + filename + ".hlsl";
    auto tpl = _env.parse_template(path);
    _templateMap[filename] = tpl;
  }
}

void ShaderGenerator::addTemplate(const std::string &name) {
  if (_templateMap.count(name)) { return; }

  auto path = TEMPLATE_ROOT + name;
  _templateMap[name] = _env.parse_template(path);
  _addTemplateCallback(name);
}

json ShaderGenerator::_getJSONForCaps(ShaderCapsSetPtr caps) const {
  json result;

  for (auto cap : CAPS_TO_PARAM_MAP) {
    auto scap = cap.first;
    bool hasCap = caps->hasCap(scap);
    auto key = cap.second;
    result[key] = hasCap;
  }

  return result;
}

ShaderPtr ShaderGenerator::getShaderWithCaps(const ShaderCapsSet &caps, const std::string &rootTemplate) const {
  ShaderPtr result;

  auto &shaderCache = _shaders[rootTemplate];
  auto iterator = shaderCache.find(caps.getBitmask());
  if (iterator == shaderCache.end()) {
    result = std::make_shared<Shader>(Engine::Get());
	result->loadFromFile(rootTemplate, std::move(caps));
    shaderCache[caps.getBitmask()] = result;
  } else {
    result = iterator->second;
  }

  return result;
}


HRESULT ShaderGenerator::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
{
	return 0;
}
HRESULT ShaderGenerator::Close(LPCVOID pData)
{
	return 0;
}


ShaderPtr ShaderGenerator::getShaderWithCaps(const ShaderCapsSet &caps) const {
  return getShaderWithCaps(caps, "resources/shaders/root.hlsl");
}
