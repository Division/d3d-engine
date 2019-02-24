#include "Engine.h"
#include "scene/Scene.h"

#include "EngineTypes.h"

class Game : public IGame {
public:
	~Game() = default;
	void init();
	void update(float dt);

private:
	std::shared_ptr<Scene> _scene;
	CameraPtr _camera;
	MeshObjectPtr _obj1;
};
