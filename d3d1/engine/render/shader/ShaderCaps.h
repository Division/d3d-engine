//
// Created by Sidorenko Nikita on 11/21/18.
//

#ifndef CPPWRAPPER_SHADERCAPS_H
#define CPPWRAPPER_SHADERCAPS_H

#include <memory>
#include <unordered_set>
#include "utils/CapsSet.h"

//---------------------
// Shader Caps

enum class ShaderCaps : uint32_t {
  Color = 0,
  ObjectData,
  VertexColor,
  PointSize,
  Billboard,
  Lighting,
  Texture0,
  Texture1,
  NormalMap,
  SpecularMap,
  ProjectedTexture,
  Skinning,
  Count
};

typedef CapsSet<ShaderCaps> ShaderCapsSet;

typedef std::shared_ptr<ShaderCapsSet> ShaderCapsSetPtr;

#endif //CPPWRAPPER_SHADERCAPS_H
