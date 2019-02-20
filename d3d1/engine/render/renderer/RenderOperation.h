//
// Created by Sidorenko Nikita on 4/13/18.
//

#ifndef CPPWRAPPER_RENDEROPERATION_H
#define CPPWRAPPER_RENDEROPERATION_H

#include "render/mesh/Mesh.h"
#include "EngineMath.h"
#include "render/material/Material.h"
#include "render/buffer/MultiBufferAddress.h"
#include "render/shader/ConstantBufferStruct.h"
#include "EngineTypes.h"

struct RenderOperation {
  MeshPtr mesh;
  MaterialPtr material;
  ConstantBufferStruct::ObjectParams *objectParams = nullptr;
  ConstantBufferStruct::SkinningMatrices *skinningMatrices = nullptr;

  //GLenum mode = GL_TRIANGLES;
  bool depthTest = true;
  MultiBufferAddress objectParamsBlockOffset;
  MultiBufferAddress skinningOffset;
  std::string *debugInfo = nullptr;
};

#endif //CPPWRAPPER_RENDEROPERATION_H
