#pragma once

#include <vector>
#include <chrono>
#include <unordered_map>

#include "system.h"

typedef std::chrono::duration<float> Duration;

namespace Engine
{
    class Settings;

    class Game
    {
    public:
        /**
         * @brief Game
         * @param argument counter
         * @param argument values
         */
        Game(int argc, char *argv[]);
        ~Game();

        /**
         * @brief updates the physics
         * @param delta time
         */
        void update(const Duration &dt);

#ifdef ZE_GAME
        /**
         * @brief render
         * @param alpha
         */
        void render(float alpha);
#endif

        /**
         * @brief main game loop
         */
        void mainLoop();

        /**
         * @brief adds a System
         * @param system to be added
         */
        void add(const System &system);

    private:
        /**
         * @brief Stores all available systems
         */
        std::vector<System> m_Systems;

        /**
         * @brief Global game settings
         */
        Settings *m_pSettings;
    };
}
