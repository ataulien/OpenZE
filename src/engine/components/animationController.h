#pragma once
#include "engine/components.h"
#include "engine/game/animHandler.h"

namespace Engine
{
	namespace Components
	{
		/**
		* @brief Handles current animations for the entity. Stores the results inside the instance-data-
		*/
		struct AnimationController
		{
			enum { MASK = C_ANIM_CONTROLLER };

			/**
			 * @brief Storage for animations of this model
			 */
			AnimHandler animHandler;

			void cleanUp()
			{
				
			}
		};
	}
}
