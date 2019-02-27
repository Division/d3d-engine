#pragma once

#include <unordered_map>
#include <vector>
#include <utility>
#include <d3d11.h>
#include "render/mesh/VertexAttrib.h"
#include "render/shader/ShaderCaps.h"
#include "render/mesh/Mesh.h"
#include "render/shader/Shader.h"
#include "Engine.h"

class InputLayoutCache {
public:
	typedef std::pair<VertexAttribSet::Bitmask, ShaderCapsSet::Bitmask> Key;
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayout;

	struct KeyHasher {
		std::size_t operator()(const Key &k) const {
			return k.first ^ !k.second;
		}
	};
	
	ID3D11InputLayout *getLayout(const MeshPtr mesh, const ShaderPtr shader) const {
		Key key = std::make_pair(mesh->vertexAttribSet().getBitmask(), shader->capsSet().getBitmask());
		
		auto iterator = _cache.find(key);
		if (iterator != _cache.end()) {
			return iterator->second;
		} else {
			auto result = _createLayout(mesh, shader);
			_cache[key] = result;
			return result;
		}
	}

	~InputLayoutCache() {
		for (auto layout : _cache) {
			SafeRelease(&layout.second);
		}
	}

private:
	ID3D11InputLayout *_createLayout(const MeshPtr mesh, const ShaderPtr shader) const {
		auto &shaderCaps = shader->capsSet();
		auto &vertexAttribSet= shader->vertexAttribSet();
		InputLayout layout;

		if (vertexAttribSet.hasCap(VertexAttrib::Position)) {
			layout.push_back(
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, (UINT)mesh->vertexOffsetBytes(), D3D11_INPUT_PER_VERTEX_DATA, 0 }
			);
		}
		if (vertexAttribSet.hasCap(VertexAttrib::TexCoord0)) {
			layout.push_back(
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (UINT)mesh->texCoordOffsetBytes(), D3D11_INPUT_PER_VERTEX_DATA, 0 }
			);
		}

		return shader->createInputLayout(&layout[0], layout.size());
	}

private:
	mutable std::unordered_map<Key, ID3D11InputLayout *, KeyHasher> _cache;
};