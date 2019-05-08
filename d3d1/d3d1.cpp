// d3d1.cpp : Defines the entry point for the application.
//

#include "d3d1.h"
#include "engine/Engine.h"
#include "game/Game.h"
#include "system/ErrorDump.h"

std::shared_ptr<Game> game;
std::unique_ptr<Engine> engine;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	ErrorDump::setup();

	game = std::make_shared<Game>();
	engine = std::make_unique<Engine>(hInstance, 1920, 1080, game);
	//engine = std::make_unique<Engine>(hInstance, 800, 600, game);
	engine->startLoop();

	ErrorDump::close();

    return 0;
}
