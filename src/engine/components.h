#pragma once

#ifdef ZE_GAME
#define COMPONENTS Components::Collision, Components::Movement, Components::Visual
#else
#define COMPONENTS Components::Collision, Components::Movement
#endif

namespace Engine
{
	/**
	 * @brief Enum of all registered components
	 */
    enum EComponents
    {
        C_NONE = 0,
		C_IN_USE = 1 << 0,
        C_COLLISION = 1 << 1,
        C_VISUAL = 1 << 2,
        C_MOVEMENT = 1 << 3,
		C_WORLDOBJECT = 1 << 4,
    };
}
