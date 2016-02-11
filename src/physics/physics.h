#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

namespace Physics
{
    class Physics
    {
    public:
        /**
         * @brief Physics
         */
        Physics(float gravity = -9.81f);

        /**
         * @brief update
         * @param dt
         */
        void update(double dt);

        /**
         * @brief init
         */
        void init();

        void addRigidBody(btRigidBody *pRigidBody);

    private:
        btDbvtBroadphase m_Broadphase;
        btDefaultCollisionConfiguration m_CollisionConfiguration;
        btCollisionDispatcher m_Dispatcher;
        btSequentialImpulseConstraintSolver m_Solver;
        btDiscreteDynamicsWorld m_DynamicsWorld;
    };
}
