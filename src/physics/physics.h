#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#include "utils/queue.h"
#include "utils/mathlib.h"
#include "collisionshape.h"

namespace Physics
{
	struct RayTestResult
	{
		/** 
		 * @brief Position of where the ray hit
		 */
		Math::float3 hitPosition;

		/**
		 * @brief Index of the trianlge the ray has potentially hit
		 */
		uint32_t hitTriangleIndex;

		/**
		 * @brief Userflags set in the hit rigidbody
		 */
		uint32_t hitFlags;
	};

    class Physics
    {
        friend class RigidBody;
    public:
        /**
         * @brief Physics
         */
        Physics(float gravity = -10.f);

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
        void updateRigidBodies();

		/**
		 * @brief Shoots a simple trace through the physics-world
		 * @param Flags to allow to be traced. All objects with none of the given flags set will be ignored.
		 * @return Information about the hitpoint
		 */
		RayTestResult rayTest(const Math::float3& start, const Math::float3& end, ECollisionType filterType = CT_Any);

        btDiscreteDynamicsWorld *world();
    private:
        btDbvtBroadphase m_Broadphase;
        btDefaultCollisionConfiguration m_CollisionConfiguration;
        btCollisionDispatcher m_Dispatcher;
        btSequentialImpulseConstraintSolver m_Solver;
        btDiscreteDynamicsWorld m_DynamicsWorld;

        Utils::Queue<btRigidBody *> m_RigidBodyQueue;
    };
}
