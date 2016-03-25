#include "utils/logger.h"

#include <iostream>
#include <string>
#include <functional>

#ifdef ZE_GAME
#include "game/gameengine.h"
#elif ZE_SERVER
#include "engine.h"
#endif

#ifdef __ANDROID__
#include "renderer/EGL_window.h"
#else
#include "renderer/GLFW_window.h"
#endif

#ifdef __ANDROID__
void android_main(android_app* state)
#else
int main(int argc, char *argv[])
#endif
{
#ifndef __ANDROID__ 
    Utils::Log::Clear();
#endif


#if defined(__ANDROID__)

	// Make sure glue isn't stripped.
	app_dummy();

	__android_log_print(ANDROID_LOG_INFO, "OpenZE", "HolyShit you did it !");

	LogInfo() << "Starting OpenZE...";

	Renderer::EGL_Window window("OpenZE", state);
	Engine::GameEngine game(0, nullptr, &window);
#elif defined ZE_GAME
	Renderer::GLFW_Window window(200, 200, 1280, 720, "OpenZE");
    Engine::GameEngine game(argc, argv, &window);
#elif defined(ZE_SERVER)
    Engine::Engine game(argc, argv);
#endif
    game.init();
    game.mainLoop();

#ifndef __ANDROID__
    return 0;
#endif
}
