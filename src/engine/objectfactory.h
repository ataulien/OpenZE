#pragma once

#include <vector>

#include "components.h"
#include "components/collision.h"
#include "components/visual.h"
#include "utils/vector.h"
#include "utils/logger.h"
#include "utils/mathlib.h"

namespace Engine
{
    class Engine;

    /**
     * @brief The ObjectFactory class creates and stores game objects
     */
    class ObjectFactory
    {
        friend class Engine;
    public:
        ObjectFactory(Engine *pEngine, uint32_t objCount = 0);

        uint32_t createEntity();

		uint32_t test_createPhysicsEntity(const Math::float3& position, const Math::float3& impulse);

        void destroyEntity(uint32_t entity);

        uint32_t createObject();

        const Utils::Vector<EComponents> &getMasks(){ return m_Mask; }
        Components::Collision &getCollision(uint32_t objectID) { return m_Collision.m_Data[objectID]; }
#ifdef ZE_GAME
        Components::Visual &getVisual(uint32_t objectID) { return m_Visual.m_Data[objectID]; }
#endif

    private:
        Engine *m_pEngine;
        /**
         * @brief stores the masks for the components
         */
        Utils::Vector<EComponents> m_Mask;

        /**
         * @brief Vectors to store the component data
         */
        Utils::Vector<Components::Collision> m_Collision;
#ifdef ZE_GAME
        Utils::Vector<Components::Visual> m_Visual;
#endif
        Utils::Vector<Components::Attributes> m_Attributes;
        Utils::Vector<Components::AI> m_AI;
    };
}
