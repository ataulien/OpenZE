#include <iostream>
#include <string>

#include "game.h"

int main(int argc, char *argv[])
{
#ifdef ZE_GAME
    std::cout << "Hello OpenZE!" << std::endl;
#elif ZE_SERVER
    std::cout << "Hello OZerver!" << std::endl;
#endif

#ifdef ZE_DEBUG
    std::cerr << "Da is was kaputt" << std::endl;
#endif

    Engine::Game game(argc, argv);
    game.mainLoop();

    return 0;
}
