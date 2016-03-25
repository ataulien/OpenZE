#pragma once

#include "components.h"

namespace Engine
{
    namespace Components
    {
		/**
		 * @brief Controllable movement for the corresponding entity
		 */
        struct Movement
        {
            enum { MASK = C_MOVEMENT };

            void cleanUp()
            {
            }
        };
    }
}
