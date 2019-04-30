//
// Created by Sidorenko Nikita on 4/21/18.
//

#ifndef CPPWRAPPER_LIGHTGRID_H
#define CPPWRAPPER_LIGHTGRID_H

#include <vector>
#include "EngineTypes.h"
#include "EngineMath.h"
#include <memory>

class ICameraParamsProvider;
class D3DStructuredBuffer;

struct LightGridCell {
	int offset;
	std::vector<LightObjectPtr> pointLights;
	std::vector<LightObjectPtr> spotLights;
	std::vector<ProjectorPtr> projectors;
	std::vector<ProjectorPtr> decals;
};

class LightGrid {
public:
	explicit LightGrid(unsigned int cellSize = 32);
	~LightGrid() = default;

	void update(unsigned int screenWidth, unsigned int screenHeight);
	void appendLights(const std::vector<LightObjectPtr> &lights, std::shared_ptr<ICameraParamsProvider> camera);
	void appendProjectors(const std::vector<ProjectorPtr> &projectors, std::shared_ptr<ICameraParamsProvider> camera);
	void upload();
	void bindBuffers(ID3D11DeviceContext1 *context);
	//void setDebugDraw(std::shared_ptr<DebugDraw> debugDraw);

private:
	std::unique_ptr<D3DMemoryBuffer> _lights;
	std::unique_ptr<D3DMemoryBuffer> _projectors;
	std::shared_ptr<D3DStructuredBuffer> _lightIndex;
	std::shared_ptr<D3DStructuredBuffer> _lightGrid;

	uint32_t _lightCount;
	uint32_t _projectorCount;

	unsigned int _cellSize;
	unsigned int _cellsX = 0;
	unsigned int _cellsY = 0;
	std::vector<LightGridCell> _cells;
	unsigned int _lightGridBlock;
	unsigned int _lightIndexBlock;
	//std::shared_ptr<DebugDraw> _debugDraw;

private:
	LightGridCell *_getCellByXY(int x, int y) { return &_cells[x + y * _cellsX]; };
	void _clearCells();
	void _appendItem(const std::shared_ptr<ICameraParamsProvider> camera, const std::vector<vec3> &edgePoints,
					 std::function<void(LightGridCell *)> callback);

	// Temporary vector to pass data through the functions
	// Placed here instead of the stack to reduce heap allocations
	std::vector<vec3> _lightEdges;
};


#endif //CPPWRAPPER_LIGHTGRID_H
