#include <iostream>
#include <vector>
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
        Physics(float gravity = -9.81f) :
            m_Dispatcher(&m_CollisionConfiguration),
            m_DynamicsWorld(&m_Dispatcher, &m_Broadphase, &m_Solver, &m_CollisionConfiguration)
        {
            btGImpactCollisionAlgorithm::registerAlgorithm(&m_Dispatcher);
            m_DynamicsWorld.setGravity(btVector3(0, gravity, 0));
        }

        /**
         * @brief update
         * @param dt
         */
        void update(float dt)
        {
            m_DynamicsWorld.stepSimulation(dt, 10);
        }

        /**
         * @brief init
         */
        void init()
        {
        }

        void addRigidBody(btRigidBody *pRigidBody)
        {
            m_DynamicsWorld.addRigidBody(pRigidBody);
        }

    private:
        btDbvtBroadphase m_Broadphase;
        btDefaultCollisionConfiguration m_CollisionConfiguration;
        btCollisionDispatcher m_Dispatcher;
        btSequentialImpulseConstraintSolver m_Solver;
        btDiscreteDynamicsWorld m_DynamicsWorld;
    };
}

int main()
{
    Physics::Physics system;

    btStaticPlaneShape groundShape(btVector3(0.0f, 1.0f, 0.0f), 1.0f);
    btDefaultMotionState groundMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, -1.0f, 0.0f)));
    btRigidBody groundRigidBody(0, &groundMotionState, &groundShape, btVector3(0, 0, 0));
    system.addRigidBody(&groundRigidBody);

    btSphereShape fallShape(1);
    btDefaultMotionState fallMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    fallShape.calculateLocalInertia(mass, fallInertia);
    btRigidBody fallRigidBody(mass, &fallMotionState, &fallShape, fallInertia);
    system.addRigidBody(&fallRigidBody);

    for(int i = 0; i < 550; ++i)
    {
        system.update(1 / 128.0f);

        btTransform trans;
        fallRigidBody.getMotionState()->getWorldTransform(trans);
        std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
    }

    return 0;
}
