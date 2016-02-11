#pragma once

#include <vector>
#include <chrono>
#include <unordered_map>

#include "objectfactory.h"
#include "physics/physics.h"

typedef std::chrono::duration<float> Duration;

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
        void updatePhysics(const Duration &dt);

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

    private:
        /**
         * @brief Global game settings
         */
        Settings *m_pSettings;

        Physics::Physics m_PhysicsSystem;
    };
}