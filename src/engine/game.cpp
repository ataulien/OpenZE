#include <iostream>

#include "game.h"
#include "settings.h"

Engine::Game::Game(int argc, char *argv[]) :
    m_pSettings(new Settings(argc, argv))
{
}

Engine::Game::~Game()
{
    delete m_pSettings;
}

void Engine::Game::update(const Duration &dt)
{
    std::cout << __PRETTY_FUNCTION__ << dt.count() << std::endl;
}

#ifdef ZE_GAME
void Engine::Game::render(float alpha)
{
    std::cout << __PRETTY_FUNCTION__ << alpha << std::endl;
}
#endif

void Engine::Game::mainLoop()
{
    typedef std::chrono::high_resolution_clock Time;

    const float fps = 100;
    const Duration dt = std::chrono::duration<float>(1 / fps);
    Duration accumulator;

    auto frameStart = Time::now();

    while(true)
    {
        auto currentTime = Time::now();

        accumulator += currentTime - frameStart;

        frameStart = currentTime;

        if(accumulator.count() > 0.05f)
            accumulator = std::chrono::duration<float>(0.05f);

        while(accumulator > dt)
        {
            update(dt);
            accumulator -= dt;
        }

#ifdef ZE_GAME
        const float alpha = accumulator / dt;

        render(alpha);
#endif
    }
}

void Engine::Game::add(const Engine::System &system)
{
    m_Systems.push_back(system);
}
