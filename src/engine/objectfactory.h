#pragma once

#include <vector>
#include <functional>

#include "objecthandle.h"
#include "objectstorage.h"
#include "components.h"
#include "utils/logger.h"
#include "utils/mathlib.h"

namespace Engine
{
    class Engine;

    /**
     * @brief The ObjectFactory class creates and stores entities
     */
    class ObjectFactory
    {
        friend class Engine;
    public:
        ObjectFactory(Engine *pEngine, uint32_t initialSize = 0) :
            m_pEngine(pEngine),
            m_Storage(initialSize)
        {
        }

        ~ObjectFactory()
        {
        }

        void test_createObjects();

        void test_createPhysicsEntity(const Math::float3& position, const Math::float3& impulse);

        ObjectStorage<COMPONENTS> &storage()
        {
            return m_Storage;
        }

        void cleanUp();

    private:

        /**
         * @brief a pointer to the engine object
         */
        Engine *m_pEngine;

        /**
         * @brief storage class for all components
         */
        ObjectStorage<COMPONENTS> m_Storage;
    };
}
