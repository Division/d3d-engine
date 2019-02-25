#include "ShaderResource.h"
#include <map>

const std::map<ShaderResourceName, unsigned int> SHADER_SAMPLER_REGISTERS = {
	{ ShaderResourceName::Texture0, 0 },
	{ ShaderResourceName::Texture1, 1 },
	{ ShaderResourceName::NormalMap, 2 },
	{ ShaderResourceName::LightGrid, 3 },
	{ ShaderResourceName::LightIndices, 4 },
	{ ShaderResourceName::SpecularMap, 5 },
	{ ShaderResourceName::ShadowMap, 7 },
	{ ShaderResourceName::ProjectorTexture, 8 },
};


const std::map<ConstantBufferName, unsigned int> CONSTANT_BUFFER_REGISTERS = {
	{ ConstantBufferName::ObjectParams, 0 },
	{ ConstantBufferName::SkinningMatrices, 1 },
	{ ConstantBufferName::Light, 2 },
	{ ConstantBufferName::Camera, 3 },
	{ ConstantBufferName::Projector, 4 }
};