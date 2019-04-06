#include "Engine.h"
#include "scene/Scene.h"

#include "EngineTypes.h"

class FollowCamera;
class Level;
class SpriteSheet;
class PlayerController;

class Game : public IGame {
public:
	~Game() = default;
	void init();
	void update(float dt);

private:
	bool _cameraControl = false;
	std::shared_ptr<SpriteSheet>_spritesheet;
	std::shared_ptr<Level> _level;
	std::shared_ptr<Scene> _scene;
	std::shared_ptr<FollowCamera> _camera;
	std::shared_ptr<PlayerController> _player;
	ModelBundlePtr _playerModel;
	MeshObjectPtr _obj1;
	MeshObjectPtr _obj2;
};
