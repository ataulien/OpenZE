#pragma once

#include "engine.h"
#include "utils/GLFW_window.h"
#include "utils/mathlib.h"
//#include "sound/soundmanager.h"

#include "zenWorld.h"

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

        /**
         * @brief m_SoundManager
         */
        //Sound::SoundManager m_SoundManager;

		/**
		 * @brief testing only
		 */
		float m_CameraAngle;
		float m_CameraZoom;
        Math::float3 m_CameraCenter;
		ZenWorld* m_TestWorld;
    };
}
