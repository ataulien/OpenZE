#include <iostream>
#include <string>
#include <utils/SFML_window.h>
#include <utils/logger.h>

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

	Utils::Log::Clear();
	LogInfo() << "Hello log!";
	LogWarn() << "Hello warning!";
	LogError() << "Hello error!";

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

    return 0;
}
