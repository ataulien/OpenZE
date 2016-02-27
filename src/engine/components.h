#pragma once

#ifdef ZE_GAME
#define COMPONENTS Components::Collision, Components::Movement, Components::Visual
#else
#define COMPONENTS Components::Collision, Components::Movement
#endif

namespace Engine
{
    enum EComponents
    {
        C_NONE = 0,
        C_COLLISION = 1 << 0,
        C_VISUAL = 1 << 1,
        C_MOVEMENT = 1 << 2,
    };
}
