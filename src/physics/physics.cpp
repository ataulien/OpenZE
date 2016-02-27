#include "physics.h"

Physics::Physics::Physics(float gravity) :
    m_Dispatcher(&m_CollisionConfiguration),
    m_DynamicsWorld(&m_Dispatcher, &m_Broadphase, &m_Solver, &m_CollisionConfiguration)
{
    btGImpactCollisionAlgorithm::registerAlgorithm(&m_Dispatcher);
    m_DynamicsWorld.setGravity(btVector3(0, gravity, 0));
}

void Physics::Physics::update(double dt)
{
    m_DynamicsWorld.stepSimulation((btScalar)dt, 10);
}

void Physics::Physics::init()
{

}

void Physics::Physics::addRigidBody(btRigidBody *pRigidBody)
{
    m_RigidBodyQueue.enqueue(pRigidBody);
}

void Physics::Physics::updateRigidBodies()
{
    btRigidBody *pRigidBody;
    while(m_RigidBodyQueue.try_dequeue(pRigidBody))
        m_DynamicsWorld.addRigidBody(pRigidBody);
}

btDiscreteDynamicsWorld *Physics::Physics::world()
{
    return &m_DynamicsWorld;
}
