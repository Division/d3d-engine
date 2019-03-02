#include "Engine.h"
#include "scene/Scene.h"

#include "EngineTypes.h"

class FollowCamera;
class Level;
class SpriteSheet;

class Game : public IGame {
public:
	~Game() = default;
	void init();
	void update(float dt);

private:
	std::shared_ptr<SpriteSheet>_spritesheet;
	std::shared_ptr<Level> _level;
	std::shared_ptr<Scene> _scene;
	std::shared_ptr<FollowCamera> _camera;
	MeshObjectPtr _obj1;
	MeshObjectPtr _obj2;
};
