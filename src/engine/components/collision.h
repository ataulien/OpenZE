#pragma once

#include "physics/rigidbody.h"

namespace Engine
{
    namespace Components
    {
        struct Collision
        {
            Physics::RigidBody rigidBody;

            void cleanUp()
            {
                rigidBody.cleanUp();
            }
        };
    }
}
