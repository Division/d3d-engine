#include "Engine.h"

class Game : public IGame {
public:
	~Game() = default;
	void init(std::shared_ptr<Engine> engine);
	void update(float dt);
};