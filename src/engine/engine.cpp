#include <iostream>

#include "engine.h"
#include "settings.h"

Engine::Engine::Engine(int argc, char *argv[]) :
    m_Factory(this),
    m_pSettings(new Settings(argc, argv))
{
}

Engine::Engine::~Engine()
{
    delete m_pSettings;
}

void Engine::Engine::updatePhysics(const Duration &dt)
{
    m_PhysicsSystem.update(dt.count());
}

void Engine::Engine::init()
{
}

bool Engine::Engine::render(float alpha)
{
    (void) alpha;
    return true;
}

void Engine::Engine::mainLoop()
{
    m_Factory.createObject();
    typedef std::chrono::high_resolution_clock Time;

    const float fps = 100;
    const Duration dt = Duration(1 / fps);
    Duration accumulator = Duration::zero();

    auto frameStart = Time::now();

    bool isRunning = true;
    while(isRunning)
    {
        auto currentTime = Time::now();

        accumulator += currentTime - frameStart;

        frameStart = currentTime;

        if(accumulator > Duration(0.2f))
            accumulator = Duration(0.2f);

        while(accumulator > dt)
        {
            updatePhysics(dt);
            accumulator -= dt;
        }

        const float alpha = accumulator / dt;

        isRunning = render(alpha);
    }
}

Physics::Physics *Engine::Engine::physicsSystem()
{
    return &m_PhysicsSystem;
}
