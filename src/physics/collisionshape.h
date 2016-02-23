#pragma once 

#include <btBulletCollisionCommon.h>

namespace Physics
{
    class CollisionShape
    {
        friend class RigidBody;
    public:
        CollisionShape(btCollisionShape *pCollisionShape) : m_pShape(pCollisionShape)
        {
        }

    private:
        btCollisionShape *shape()
        {
            return m_pShape;
        }

        btCollisionShape *m_pShape;
    };
}
