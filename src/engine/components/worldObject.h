#pragma once

#include "components.h"
#include "utils/mathlib.h"


namespace Engine
{
	namespace Components
	{
		/**
		 * @brief Component containing additional information about a usual game-object
		 */
		struct WorldObject
		{
			enum { MASK = C_WORLDOBJECT };

			/**
			 * @brief The worldmesh-triangle this object is currently standing on
			 */
		};
	}
}

