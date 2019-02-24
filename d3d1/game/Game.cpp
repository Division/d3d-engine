#include "Game.h"
#include "objects/Camera.h"
#include "objects/MeshObject.h"
#include "utils/MeshGeneration.h"

void Game::init()
{
	_scene = std::make_shared<Scene>();
	_camera = CreateGameObject<Camera>();
	_obj1 = CreateGameObject<MeshObject>();
	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh());
	MeshGeneration::generateQuad(mesh, vec2(1, 1));
	mesh->createBuffer();
	_obj1->mesh(mesh);
}

void Game::update(float dt)
{

}
