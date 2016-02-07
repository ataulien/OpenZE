#include <iostream>
#include <string>
#include <functional>

#include <utils/SFML_window.h>
#include <utils/logger.h>

#include "engine.h"

int main(int argc, char *argv[])
{
#ifdef ZE_GAME
    //Test
	Utils::Log::Clear();
	LogInfo() << "Hello log!";
	LogWarn() << "Hello warning!";
	LogError() << "Hello error!";

    Utils::SFML_Window wnd(200, 200, 800, 600, "OpenZE");

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

    Engine::Engine game(argc, argv);
    game.mainLoop();

    return 0;
}
