#pragma once

#include <vector>
#include <chrono>
#include <unordered_map>

#include "objectfactory.h"
#include "physics/physics.h"
#include "utils/timer.h"
#include "components/collision.h"
#include "components/visual.h"



namespace Engine
{
    class Settings;

    /**
     * @brief The Engine class
     */
    class Engine
    {
    public:
        /**
         * @brief Engine
         * @param argument counter
         * @param argument values
         */
        Engine(int argc, char *argv[]);
        virtual ~Engine();

        /**
         * @brief updates the physics
         * @param delta time
         */
        void updatePhysics(const std::chrono::duration<double> &dt);

        /**
         * @brief render
         * @param alpha
         * @return
         */
        virtual bool render(float alpha);

        /**
         * @brief main game loop
         */
        void mainLoop();

        /**
         * @brief physicsSystem
         * @return
         */
        Physics::Physics *physicsSystem();

        /**
         * @brief init
         */
        virtual void init();

    protected:
        /**
         * @brief the object factory creates all game objects
         */
        ObjectFactory m_Factory;

    protected:
        /**
         * @brief Global game settings
         */
        Settings *m_pSettings;

		/**
		 * @brief Main physics system
		 */
        Physics::Physics m_PhysicsSystem;

		/**
		 * @brief Timer for the mainloop
		 */
		Utils::Timer<double, 20> m_MainLoopTimer;
    };
}
