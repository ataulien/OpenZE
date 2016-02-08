#pragma once

class btRigidBody;
class btCollisionShape;
class btMotionState;

namespace Engine
{
    namespace Components
    {
        struct Collision
        {
            btRigidBody *pRigidBody;
            btCollisionShape *pCollisionShape;
            btMotionState *pMotionState;
        };
    }
}
