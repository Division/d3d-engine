#pragma once

enum class ShaderResourceName : int {
	Texture0,
	Texture1,
	NormalMap,
	SpecularMap,
	ShadowMap,
	ProjectorTexture,
	LightGrid,
	LightIndices
};

enum class ConstantBufferName : int {
	ObjectParams,
	SkinningMatrices,
	Light,
	Camera,
	Projector
};

extern const std::map<ShaderResourceName, unsigned int> SHADER_SAMPLER_REGISTERS;
extern const std::map<ConstantBufferName, unsigned int> CONSTANT_BUFFER_REGISTERS;