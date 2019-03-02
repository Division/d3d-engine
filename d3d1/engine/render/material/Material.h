#pragma once

#include "EngineTypes.h"
#include "render/shader/ShaderCaps.h"
#include <unordered_set>
#include <vector>

class Material {
public:
	friend class SceneRenderer;

	Material() = default;

	void texture0(TexturePtr texture);
	TexturePtr texture0() const { return _texture0; };
	
	void normalMap(TexturePtr normalMap);
	TexturePtr normalMap() const { return _normalMap; };

	void lightingEnabled(bool lightingEnabled);
	bool lightingEnabled() const { return _lightingEnabled; }

	void vertexColorEnabled(bool vertexColorEnabled);
	bool vertexColorEnabled() const { return _vertexColorEnabled; }
	
	const ShaderCapsSet &shaderCaps() const { return _shaderCaps; }
	const ShaderCapsSet &shaderCapsSkinning() const { return _shaderCapsSkinning; }

protected:
	void _updateCaps();
	void _capsInitialized();

protected:
	static std::unordered_set<ShaderCapsSet::Bitmask> _capsVariations;
	static std::vector<ShaderCapsSet::Bitmask> _uninitializedCaps;

	bool _capsDirty = true;
	ShaderCapsSet _shaderCaps;
	ShaderCapsSet _shaderCapsSkinning;

	bool _hasObjectParams = true;

	bool _hasTexture0 = false;
	TexturePtr _texture0;

	bool _hasNormalMap = false;
	TexturePtr _normalMap;

	bool _lightingEnabled = false;
	bool _vertexColorEnabled = false;
};