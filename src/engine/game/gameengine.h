#pragma once

#include "engine.h"
#include "renderer/GLFW_window.h"
#include "utils/mathlib.h"
//#include "sound/soundmanager.h"

#include "zenWorld.h"
#include "zenconvert/export.h"


RAPI::RBuffer* loadZENMesh(const std::string& file, std::vector<Math::float3>& zenVertices, std::vector<uint32_t>& zenIndices, float scale = ZEN_SCALE_FACTOR);

namespace Engine
{
    class GameEngine : public Engine
    {
    public:
        GameEngine(int argc, char *argv[], Renderer::Window* window);
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

		// TODO: Remove this
		const Math::float3& getCameraCenter() { return m_CameraCenter; }

    private:
        /**
         * @brief Main game window
         */
        Renderer::Window* m_Window;

        /**
         * @brief m_SoundManager
         */
        //Sound::SoundManager m_SoundManager;

		/**
		 * @brief testing only
		 */
		float m_CameraAngle;
		float m_CameraZoom;
		bool m_IsFlying;
        Math::float3 m_CameraCenter;
		ZenWorld* m_TestWorld;
		RAPI::RBuffer* m_pCameraBuffer;
    };
}
