#pragma once

#include <LinearMath/btDefaultMotionState.h>
#include <shared_mutex>
#include "utils/mathlib.h"

namespace Physics
{
    class MotionState : public btDefaultMotionState
    {
    public:
        MotionState(const Math::float3 &pos);

        Math::float3 pos();
        Math::float4 rot();
        void openGLMatrix(float *m);

    protected:
        virtual void getWorldTransform(btTransform &centerOfMassWorldTrans) const;
        virtual void setWorldTransform(const btTransform &centerOfMassWorldTrans);

    private:
        mutable std::shared_timed_mutex m_Mutex;
    };
}
