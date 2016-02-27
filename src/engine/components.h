#pragma once

#ifdef ZE_GAME
#define COMPONENTS Components::Collision, Components::Visual
#else
#define COMPONENTS Components::Collision
#endif

namespace Engine
{
    enum EComponents
    {
        C_NONE = 0,
		C_IN_USE = 1 << 0,
        C_COLLISION = 1 << 1,
        C_VISUAL = 1 << 2,
    };
}
