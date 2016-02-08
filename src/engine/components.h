#pragma once

namespace Engine
{
    enum EComponents
    {
        C_NONE = 0,
        C_COLLISION = 1 << 0,
#ifdef ZE_GAME
        C_VISUAL = 1 << 1,
#endif
        C_ATTRIBUTES = 1 << 2,
        C_AI = 1 << 3,
    };

    namespace Components
    {
        struct Attributes
        {
        };

        struct AI
        {
        };
    }
}
