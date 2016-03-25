#pragma once

#include "physics/rigidbody.h"
#include "engine/components.h"

namespace Engine
{
    namespace Components
    {
		/**
		 * @brief Interface to the physics-engine. This component can make the entity react to
		 *		  physics or make others react to itself
		 */
        struct Collision
        {
            enum { MASK = C_COLLISION };

			/**
			 * @brief Storage for all physics-properties and handling of physical world interaction
			 */
            Physics::RigidBody rigidBody;

			/**
			* @brief Filter-group this shape belongs to. Using these flags you can determine whether this 
			*		  shape is the worldmesh or some other object for example
			*/
			Physics::ECollisionType collisionType;

            void cleanUp()
            {
                rigidBody.cleanUp();
            }
        };
    }
}
