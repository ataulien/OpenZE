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
    m_Factory.cleanUp();
    delete m_pSettings;
}

void Engine::Engine::updatePhysics(const std::chrono::duration<double> &dt)
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
    m_Factory.test_createObjects();

	// Define the updaterate for the game-logic
    const float update_fps = 64;
    auto update_dt = std::chrono::duration<double>(1 / update_fps);
	auto max_dt_seconds = std::chrono::duration<double>(0.2f);
	std::chrono::duration<double> accumulator = std::chrono::duration<double>::zero();
	std::chrono::duration<double> delta = std::chrono::duration<double>::zero();
	// Start the timer
	m_MainLoopTimer.update();

    bool isRunning = true;
    while(isRunning)
    {
		delta =  m_MainLoopTimer.update();
        accumulator += delta;

        if(accumulator > max_dt_seconds)
            accumulator = max_dt_seconds;

		// Update the physics as often as we have to
        while(accumulator > update_dt)
        {       
            accumulator -= update_dt;
        }

        // Let bullet do it's own fixed timestamp
        updatePhysics(delta);

		//updatePhysics(accumulator);
		//accumulator = std::chrono::duration<double>::zero();

		// Generate interpolation value for the renderer
        const float alpha = static_cast<float>(accumulator / update_dt);

		// Draw the current frame
        isRunning = render(alpha);
    }
}

Physics::Physics *Engine::Engine::physicsSystem()
{
    return &m_PhysicsSystem;
}
