#include "utils/logger.h"

#include <iostream>
#include <string>
#include <functional>

#ifdef ZE_GAME
#include "game/gameengine.h"
#elif ZE_SERVER
#include "engine.h"
#endif


int main(int argc, char *argv[])
{
    Utils::Log::Clear();

#ifdef ZE_GAME
    Engine::GameEngine game(argc, argv);
#elif defined(ZE_SERVER)
    Engine::Engine game(argc, argv);
#endif
    game.init();
    game.mainLoop();

    return 0;
}
