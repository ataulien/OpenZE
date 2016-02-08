#pragma once

#include "engine.h"
#include "utils/GLFW_window.h"

namespace Engine
{
    class GameEngine : public Engine
    {
    public:
        GameEngine(int argc, char *argv[]);
        virtual ~GameEngine();

        /**
         * @brief render
         * @param alpha
         */
        virtual bool render(float alpha) override;

        /**
         * @brief init
         */
        virtual void init() override;

    private:
        /**
         * @brief Main game window
         */
        Utils::GLFW_Window m_Window;
    };
}
