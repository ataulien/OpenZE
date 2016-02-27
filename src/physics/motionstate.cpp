#include "motionstate.h"

Physics::MotionState::MotionState(const Math::float3 &pos) :
    btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(pos.x, pos.y, pos.z)))
{
}

Math::float3 Physics::MotionState::pos()
{
    btTransform trans;
    getWorldTransform(trans);
    btVector3 origin = trans.getOrigin();
    return Math::float3(origin.x(), origin.y(), origin.z());
}

Math::float4 Physics::MotionState::rot()
{
    btTransform trans;
    getWorldTransform(trans);
    btQuaternion rotation = trans.getRotation();
    return Math::float4(rotation.x(), rotation.y(), rotation.z(), rotation.w());
}

void Physics::MotionState::openGLMatrix(float *m)
{
    btTransform trans;
    getWorldTransform(trans);
    trans.getOpenGLMatrix(m);
}

void Physics::MotionState::getWorldTransform(btTransform &centerOfMassWorldTrans) const
{
    std::shared_lock<std::shared_timed_mutex> shared(m_Mutex, std::defer_lock);
    btDefaultMotionState::getWorldTransform(centerOfMassWorldTrans);
}

void Physics::MotionState::setWorldTransform(const btTransform &centerOfMassWorldTrans)
{
    std::unique_lock<std::shared_timed_mutex> exclusive(m_Mutex, std::defer_lock);
    btDefaultMotionState::setWorldTransform(centerOfMassWorldTrans);
}
