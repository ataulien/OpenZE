#pragma once

#include "components.h"

namespace Engine
{
    namespace Components
    {
        struct Movement
        {
            enum { MASK = C_MOVEMENT };

            void cleanUp()
            {
            }
        };
    }
}
