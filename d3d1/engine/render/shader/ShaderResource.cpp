#include "ShaderResource.h"
#include <map>

const std::map<ShaderResourceName, uint32_t> SHADER_SAMPLER_REGISTERS = {
	{ ShaderResourceName::Texture0, 0 },
	{ ShaderResourceName::Texture1, 1 },
	{ ShaderResourceName::NormalMap, 2 },
	{ ShaderResourceName::LightGrid, 3 },
	{ ShaderResourceName::LightIndices, 4 },
	{ ShaderResourceName::SpecularMap, 5 },
	{ ShaderResourceName::ShadowMap, 7 },
	{ ShaderResourceName::ProjectorTexture, 8 },
};
