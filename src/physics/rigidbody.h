#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#include "collisionshape.h"
#include "physics.h"
#include "motionstate.h"
#include "utils/mathlib.h"

namespace Physics
{
    class CollisionShape;

    class RigidBody
    {
    public:
        RigidBody() noexcept :
            m_pDynamicsWorld(nullptr),
            m_pRigidBody(nullptr)
        {
        }

        RigidBody(const RigidBody &) noexcept :
            m_pDynamicsWorld(nullptr),
            m_pRigidBody(nullptr)
        {
        }

        RigidBody(RigidBody &&other) noexcept :
            m_pDynamicsWorld(other.m_pDynamicsWorld),
            m_pRigidBody(other.m_pRigidBody)
        {
            other.invalidate();
        }

        RigidBody &operator=(RigidBody &&other) noexcept
        {
            m_pDynamicsWorld = other.m_pDynamicsWorld;
            m_pRigidBody = other.m_pRigidBody;

            other.invalidate();
            return *this;
        }

        ~RigidBody() noexcept
        {
            cleanUp();
        }

        void invalidate()
        {
            m_pDynamicsWorld = nullptr;
            m_pRigidBody = nullptr;
        }

        bool initPhysics(Physics *pPhysicsSystem, CollisionShape &collisionShape, const Math::float3 &pos, float mass = 0)
        {
            if(m_pRigidBody)
                return false;

            btMotionState *pMotionState = new MotionState(pos);
            btVector3 inertia;
            if(mass)
                collisionShape.shape()->calculateLocalInertia(mass, inertia);

            m_pRigidBody = new btRigidBody(mass, pMotionState, collisionShape.shape(), inertia);

            pPhysicsSystem->addRigidBody(m_pRigidBody);
            m_pDynamicsWorld = pPhysicsSystem->world();

            return true;
        }

        void cleanUp()
        {
            if(m_pRigidBody && m_pDynamicsWorld)
            {
                m_pDynamicsWorld->removeRigidBody(m_pRigidBody);
                delete m_pRigidBody->getMotionState();
                delete m_pRigidBody;
                m_pRigidBody = nullptr;
                m_pDynamicsWorld = nullptr;
            }
        }

        void setFriction(float friction)
        {
            m_pRigidBody->setFriction(friction);
        }

        void setRestitution(float restitution)
        {
            m_pRigidBody->setRestitution(restitution);
        }

        void applyCentralImpulse(float x, float y, float z)
        {
            m_pRigidBody->applyCentralImpulse(btVector3(x, y, z));
        }

        void applyCentralImpulse(const Math::float3 &impulse)
        {
            m_pRigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
        }

        btMotionState *getMotionState()
        {
            return m_pRigidBody->getMotionState();
        }

    private:
        btDiscreteDynamicsWorld *m_pDynamicsWorld;
        btRigidBody *m_pRigidBody;
    };
}
