#include <iostream>
#include <string>
#include <functional>

#include <utils/GLFW_window.h>
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

<<<<<<< HEAD
    Utils::SFML_Window wnd(200, 200, 800, 600, "OpenZE");
=======
	Utils::GLFW_Window wnd(200,200, 800, 600, "OpenZE");
>>>>>>> 22bd7a036a5487131824eb3bb4b3b5f9132bc46a

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
