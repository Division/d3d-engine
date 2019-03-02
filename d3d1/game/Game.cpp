#include "Game.h"
#include "objects/Camera.h"
#include "objects/MeshObject.h"
#include "utils/MeshGeneration.h"
#include "render/material/Material.h"
#include "objects/FollowCamera.h"
#include "level/Level.h"
#include "loader/SpritesheetLoader.h"
#include "loader/TextureLoader.h"
#include "render/texture/SpriteSheet.h"

float angle = 0;

void Game::init() {
	_scene = std::make_shared<Scene>();
	_camera = CreateGameObject<FollowCamera>();

	_spritesheet = loader::loadSpritesheet("resources/common/decals.json");
	auto decals = loader::loadTexture("resources/common/" + _spritesheet->spritesheetName());
	//engine->projectorTexture(decals);

	_level = std::make_shared<Level>(_scene, _spritesheet, decals);
	_level->load("resources/level/level1.mdl");

	//_camera->setPlayer(_player);
	_obj1 = CreateGameObject<MeshObject>();
	_obj2 = CreateGameObject<MeshObject>();
	
	std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(new Mesh());
	MeshGeneration::generateQuad(mesh, vec2(1, 1));
	mesh->createBuffer();
	auto material = std::make_shared<Material>();

	_obj1->mesh(mesh);
	_obj1->material(material);
	_obj1->transform()->position(vec3(-0.3, 0, 0));
	_obj1->transform()->scale(vec3(0.6, 0.6, 0.6));
	_obj2->mesh(mesh);
	_obj2->material(material);
	_obj2->transform()->position(vec3(0.3, 0, 0));
	_obj2->transform()->scale(vec3(0.6, 0.6, 0.6));

	_camera->transform()->position(vec3(0, 0, -10));
}

void Game::update(float dt) {
	_obj1->transform()->rotate(vec3(0, 0, 1), M_PI * dt);
	_obj2->transform()->rotate(vec3(0, 0, 1), M_PI * dt * 2);
	_scene->update(dt);
	Engine::Get()->renderScene(_scene, _camera, nullptr);
}
