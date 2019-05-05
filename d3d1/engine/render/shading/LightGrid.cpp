//
// Created by Sidorenko Nikita on 4/21/18.
//

#include "Engine.h"
#include <objects/Camera.h>
#include "LightGrid.h"
#include "objects/LightObject.h"
#include "objects/Projector.h"
#include "system/Logging.h"
#include <functional>
#include "render/renderer/ICameraParamsProvider.h"
#include "render/shader/ShaderResource.h"
#include "render/buffer/D3DStructuredBuffer.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "render/shader/ConstantBufferStruct.h"
#include "DXCaps.h"

const uint32_t MAX_LIGHTS = 100;
const uint32_t LIGHTS_SIZE = ((uint32_t)ceilf(MAX_LIGHTS * sizeof(ConstantBufferStruct::Light) / 256.0f)) * 256;
const uint32_t PROJECTORS_SIZE = ((uint32_t)ceilf(MAX_LIGHTS * sizeof(ConstantBufferStruct::Light) / 256.0f)) * 256;

struct LightGridStruct {
	uint32_t offset;
	uint16_t pointLightCount;
	uint16_t spotLightCount;
	uint16_t projectorCount;
	uint16_t decalCount;
};

LightGrid::LightGrid(unsigned int cellSize) : _cellSize(cellSize) {
	_lightGrid = std::make_shared<D3DStructuredBuffer>(
		Engine::Get(), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, sizeof(LightGridStruct)
	);
	_lightIndex = std::make_shared<D3DStructuredBuffer>(
		Engine::Get(), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, sizeof(uint32_t)
	);

	// Fixed size buffers for lights/projectors
	_lights = std::make_unique<D3DMemoryBuffer>(
		Engine::Get(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, LIGHTS_SIZE
	);
	_projectors = std::make_unique<D3DMemoryBuffer>(
		Engine::Get(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, PROJECTORS_SIZE
	);

	_lightGridBlock = SHADER_SAMPLER_REGISTERS.at(ShaderResourceName::LightGrid);
	_lightIndexBlock = SHADER_SAMPLER_REGISTERS.at(ShaderResourceName::LightIndices);
}

void LightGrid::update(unsigned int screenWidth, unsigned int screenHeight) {
  auto newCellsX = (unsigned int)ceilf((float)screenWidth / (float)_cellSize);
  auto newCellsY = (unsigned int)ceilf((float)screenHeight / (float)_cellSize);

  if (newCellsX != _cellsX || newCellsY != _cellsY) {
    _cellsX = newCellsX;
    _cellsY = newCellsY;
  }

  auto size = _cellsX * _cellsY;
  if (size > _cells.size()) {
    _cells.resize(_cellsX * _cellsY);
  }

  _clearCells();
}

void LightGrid::_clearCells() {
	for (auto &cell: _cells) {
		cell.pointLights.clear();
		cell.spotLights.clear();
		cell.projectors.clear();
		cell.decals.clear();
	}
}

// Executes callback for every cell within projected edgePoints bounds
// edgePoints is vector because it has different length depending on the object
void LightGrid::_appendItem(const std::shared_ptr<ICameraParamsProvider> camera, const std::vector<vec3> &edgePoints,
                            std::function<void(LightGridCell *)> callback) {
  std::vector<vec3> projectedEdges(edgePoints.size());
  AABB bounds;

  for (int i = 0; i < edgePoints.size(); i++) {
    projectedEdges[i] = glm::project(edgePoints[i], camera->cameraViewMatrix(), camera->cameraProjectionMatrix(), camera->cameraViewport());
    if (i == 0) {
      bounds.min = projectedEdges[i];
      bounds.max = projectedEdges[i];
    } else {
      bounds.expand(projectedEdges[i]);
    }
  }

  float cellSize = (float)_cellSize;
  float lastCellX = (float)((int)_cellsX - 1);
  float lastCellY = (float)((int)_cellsY - 1);
  int extra = 3;
  auto startX = (int)round(floorf(fminf(fmaxf(bounds.min.x / cellSize - extra, 0), lastCellX)));
  auto startY = (int)round(floorf(fminf(fmaxf(bounds.min.y / cellSize - extra, 0), lastCellY)));
  auto endX = (int)round(floorf(fmaxf(fminf(bounds.max.x / cellSize + extra, lastCellX), 0)));
  auto endY = (int)round(floorf(fmaxf(fminf(bounds.max.y / cellSize + extra, lastCellY), 0)));

  if ((endX < 0) || (startX >= (int)_cellsX) || (endY < 0) || (startY >= (int)_cellsY)) {
    return; // light out of grid bounds
  }

  for (auto i = startX; i <= endX; i++ ) {
    for (auto j = startY; j <= endY; j++) {
      auto cell = _getCellByXY(i, j);
      callback(cell);
    }
  }
}

void LightGrid::appendLights(const std::vector<LightObjectPtr> &lights,
                             const std::shared_ptr<ICameraParamsProvider> camera) {
  _lightCount = lights.size();

  for (int i = 0; i < lights.size(); i++) {
	  auto &light = lights[i];
	  auto lightData = light->getLightStruct();
	  _lights->appendData(&lightData, sizeof(lightData));
	  light->index(i);
  }

  _lightEdges.resize(4);

  for (auto &light : lights) {
    vec3 position = light->transform()->worldPosition();
    float radius = light->radius();

    _lightEdges[0] = position + camera->cameraLeft() * radius;
    _lightEdges[1] = position + camera->cameraRight() * radius;
    _lightEdges[2] = position + camera->cameraUp() * radius;
    _lightEdges[3] = position + camera->cameraDown() * radius;

    _appendItem(camera, _lightEdges, [&](LightGridCell *cell) {
      switch(light->type()) {
        case LightObjectType::Spot:
          cell->spotLights.push_back(light);
          break;
        case LightObjectType::Point:
          cell->pointLights.push_back(light);
          break;
      }
    });
  }
}

void LightGrid::appendProjectors(const std::vector<ProjectorPtr> &projectors,
                                 std::shared_ptr<ICameraParamsProvider> camera) {

  _projectorCount = projectors.size();

  for (int i = 0; i < projectors.size(); i++) {
	  auto &projector = projectors[i];
	  auto projectorData = projector->getProjectorStruct();
	  _projectors->appendData(&projectorData, sizeof(projectorData));
	  projector->index(i);
  }

  for (auto &projector : projectors) {
    projector->getEdgePoints(_lightEdges);

    _appendItem(camera, _lightEdges, [&](LightGridCell *cell) {
      switch(projector->type()) {
        case ProjectorType::Projector:
          cell->projectors.push_back(projector);
          break;
        case ProjectorType::Decal:
          cell->decals.push_back(projector);
          break;
      }
    });
  }
}

// Upload grid data into the GPU buffers
void LightGrid::upload() {

  // Resize grid buffer to fit all the cell structs
  _lightGrid->resize((uint32_t)_cells.size() * sizeof(LightGridStruct));
  // Little bit unsafe but convenient way to directly modify data within the memory
  auto gridBufferPointer = (LightGridStruct *)_lightGrid->bufferPointer();

  uint32_t currentOffset = 0;
  for (int i = 0; i < _cells.size(); i++) {
    auto &cell = _cells[i];

    // Writing cell data
    // Referencing memory at the offset sizeof(LightGridStruct) * i
    gridBufferPointer[i].offset = currentOffset;
    gridBufferPointer[i].pointLightCount = (uint16_t)cell.pointLights.size();
    gridBufferPointer[i].spotLightCount = (uint16_t)cell.spotLights.size();
    gridBufferPointer[i].projectorCount = (uint16_t)cell.projectors.size();
    gridBufferPointer[i].decalCount = (uint16_t)cell.decals.size();

    // Writing indices
    // Count of light sources to put into the index data structure
    int indexDataSize = gridBufferPointer[i].pointLightCount +
                        gridBufferPointer[i].spotLightCount +
                        gridBufferPointer[i].projectorCount +
                        gridBufferPointer[i].decalCount;

	_lightIndex->resize((indexDataSize + currentOffset) * sizeof(uint32_t));
    // pointer should be obtained after resize() since resize may reallocate the data
    auto indexBufferPointer = (uint32_t *)_lightIndex->bufferPointer();

    // Indices for point lights
    for (int j = 0; j < gridBufferPointer[i].pointLightCount; j++) {
      indexBufferPointer[currentOffset + j] = (uint32_t)cell.pointLights[j]->index();
    }
    currentOffset += gridBufferPointer[i].pointLightCount;

    // Indices for spot lights
    for (int j = 0; j < gridBufferPointer[i].spotLightCount; j++) {
      indexBufferPointer[currentOffset + j] = (uint32_t)cell.spotLights[j]->index();
    }
    currentOffset += gridBufferPointer[i].spotLightCount;

    // Indices for projectors
    for (int j = 0; j < gridBufferPointer[i].projectorCount; j++) {
      indexBufferPointer[currentOffset + j] = (uint32_t)cell.projectors[j]->index();
    }
    currentOffset += gridBufferPointer[i].projectorCount;

    // Indices for decals
    for (int j = 0; j < gridBufferPointer[i].decalCount; j++) {
      indexBufferPointer[currentOffset + j] = (uint32_t)cell.decals[j]->index();
    }
    currentOffset += gridBufferPointer[i].decalCount;
  }

  _lightGrid->setDirty();
  _lightGrid->upload();
  _lightIndex->setDirty();
  _lightIndex->upload();
  _lights->upload();
  _projectors->upload();
}

void LightGrid::bindBuffers(ID3D11DeviceContext1 *context) {
	context->PSSetShaderResources(_lightGridBlock, 1, _lightGrid->shaderResourceViewPointer());
	context->PSSetShaderResources(_lightIndexBlock, 1, _lightIndex->shaderResourceViewPointer());

	// Lights
	auto constantBuffer = _lights->buffer();
	UINT firstConstant = 0;
	UINT constantCount = LIGHTS_SIZE / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;

	context->VSSetConstantBuffers1((UINT)ConstantBufferName::Light, 1, &constantBuffer, &firstConstant, &constantCount);
	context->PSSetConstantBuffers1((UINT)ConstantBufferName::Light, 1, &constantBuffer, &firstConstant, &constantCount);

	// Projectors
	constantBuffer = _projectors->buffer();
	constantCount = PROJECTORS_SIZE / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;

	context->VSSetConstantBuffers1((UINT)ConstantBufferName::Projector, 1, &constantBuffer, &firstConstant, &constantCount);
	context->PSSetConstantBuffers1((UINT)ConstantBufferName::Projector, 1, &constantBuffer, &firstConstant, &constantCount);
}

/*
void LightGrid::setDebugDraw(std::shared_ptr<DebugDraw> debugDraw) {
  _debugDraw = debugDraw;
}
*/

