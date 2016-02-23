#pragma once

#include <vector>
#include <functional>

#include "components.h"
#include "components/collision.h"
#include "components/visual.h"
#include "utils/vector.h"
#include "utils/logger.h"
#include "utils/mathlib.h"

#define OBJECT_HANDLE_COUNT_BITS 15
#define OBJECT_HANDLE_HANDLE_BITS (32 - OBJECT_HANDLE_COUNT_BITS)

namespace Engine
{
    struct ObjectHandle
    {
        union
        {
            struct
            {
                uint32_t count: OBJECT_HANDLE_COUNT_BITS;
                uint32_t offset: OBJECT_HANDLE_HANDLE_BITS;
            };
            uint32_t handle;
        };
    };

    struct Entity
    {
        Entity(uint32_t offset = 0);

        ObjectHandle handle;
        EComponents mask;
    };

    class Engine;

    /**
     * @brief The ObjectFactory class creates and stores entities
     */
    class ObjectFactory
    {
        friend class Engine;

    public:
        ObjectFactory(Engine *pEngine, uint32_t initialSize = 0);
        ~ObjectFactory();

        ObjectHandle createEntity();
        void destroyEntity(uint32_t offset);
        void destroyEntity(ObjectHandle handle);

        void test_createPhysicsEntity(const Math::float3& position, const Math::float3& impulse);

        void test_createObjects();

        Utils::Vector<Entity> &getEntities();

        Components::Collision *getCollision(const ObjectHandle &handle);
        Components::Collision *addCollisionComponent(const ObjectHandle &handle);
        bool removeCollisionComponent(const ObjectHandle &handle);

#ifdef ZE_GAME
        Components::Visual *getVisual(const ObjectHandle &handle);
        Components::Visual *addVisualComponent(const ObjectHandle &handle);
        bool removeVisualComponent(const ObjectHandle &handle);
#endif

        void cleanUp();

    private:
        /**
         * @brief
         * @return object offset
         */
        uint32_t firstFreeEntityOffset();

        /**
         * @brief a pointer to the engine object
         */
        Engine *m_pEngine;

        /**
         * @brief this vector stores all entity objects
         */
        Utils::Vector<Entity> m_Entities;

        /**
         * @brief this vector stores all collision objects
         */
        Utils::Vector<Components::Collision> m_CCollisions;

#ifdef ZE_GAME
        /**
         * @brief this vector stores all visual objects
         */
        Utils::Vector<Components::Visual> m_CVisuals;
#endif

        /**
         * @brief this vector stores all attribute objects
         */
        Utils::Vector<Components::Attributes> m_CAttributes;

        /**
         * @brief this vector stores all AI objects
         */
        Utils::Vector<Components::AI> m_CAis;
    };
}
