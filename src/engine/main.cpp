#include <iostream>
#include <string>
#include <functional>

#include <utils/SFML_window.h>

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

#ifdef ZE_GAME
    //Test
	Utils::SFML_Window wnd(200,200, 800, 600, "OpenZE");

	bool isRunning = true;
	while(isRunning)
	{
		wnd.pollEvent([&](Utils::Window::EEvent ev)
		{
			switch(ev)
			{
			case Utils::Window::EEvent::E_Closed:
				isRunning = false;
				break;

			case Utils::Window::E_Resized:
				std::cout << "Resized window!";
				break;
			}
		});
    }
#endif

    Engine::Game game(argc, argv);
    game.mainLoop();

    return 0;
}
