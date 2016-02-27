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

/**
* @brief Shoots a simple trace through the physics-world
* @return Distance to the hit-surface
*/
Math::float3 Physics::Physics::rayTest(const Math::float3& start, const Math::float3& end)
{
	struct FilteredRayResultCallback : public btCollisionWorld::RayResultCallback
	{
		FilteredRayResultCallback(){}
		virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
		{
			const btRigidBody* rb = btRigidBody::upcast(rayResult.m_collisionObject);
			if(rb)	
				return addSingleResult_close(rayResult, normalInWorldSpace);	

			return 0;
		}

		btVector3	m_rayFromWorld;
		btVector3	m_rayToWorld;

		btVector3	m_hitNormalWorld;
		btVector3	m_hitPointWorld;

		virtual	btScalar	addSingleResult_close(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
		{
			//caller already does the filter on the m_closestHitFraction
			btAssert(rayResult.m_hitFraction <= m_closestHitFraction);

			m_closestHitFraction = rayResult.m_hitFraction;
			m_collisionObject = rayResult.m_collisionObject;
			if (normalInWorldSpace)
			{
				m_hitNormalWorld = rayResult.m_hitNormalLocal;
			} else
			{
				///need to transform normal into worldspace
				m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
			}
			m_hitPointWorld.setInterpolate3(m_rayFromWorld,m_rayToWorld,rayResult.m_hitFraction);


			return rayResult.m_hitFraction;
		}

	};

	FilteredRayResultCallback r;
	r.m_rayFromWorld = btVector3(start.x, start.y, start.z);
	r.m_rayToWorld = btVector3(end.x, end.y, end.z);
	r.m_hitPointWorld = r.m_rayFromWorld;

	btVector3 from = {start.x, start.y, start.z};
	btVector3 to = {end.x, end.y, end.z};
	m_DynamicsWorld.rayTest(from, to, r);

	return Math::float3(r.m_hitPointWorld.x(),r.m_hitPointWorld.y(),r.m_hitPointWorld.z());
}