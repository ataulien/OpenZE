#pragma once

#include "physics/rigidbody.h"
#include "engine/components.h"

namespace Engine
{
    namespace Components
    {
        struct Collision
        {
            enum { MASK = C_COLLISION };

            Physics::RigidBody rigidBody;

            void cleanUp()
            {
                rigidBody.cleanUp();
            }
        };
    }
}
