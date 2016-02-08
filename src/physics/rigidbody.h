#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#include "collisionshape.h"
#include "motionstate.h"

namespace Physics
{
    class CollisionShape;
    class MotionState;

    class RigidBody : public btRigidBody
    {
    public:
        RigidBody() :
            btRigidBody(0, nullptr, nullptr)
        {
        }

        RigidBody(float mass, MotionState *pMotionState, CollisionShape *pCollisionShape, const btVector3 &localInertia = btVector3(0, 0, 0)) :
            btRigidBody(mass, static_cast<btMotionState *>(pMotionState), static_cast<btCollisionShape *>(pCollisionShape), localInertia)
        {
        }

        virtual ~RigidBody() override
        {
        }
    };
}
