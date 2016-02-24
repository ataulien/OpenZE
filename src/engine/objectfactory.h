#pragma once

#include <vector>
#include <functional>

#include "components.h"
#include "components/collision.h"
#include "components/visual.h"
#include "utils/vector.h"
#include "utils/logger.h"
#include "utils/mathlib.h"
#include "objectfactory.h"
#include "physics/rigidbody.h"
#include "utils/mathlib.h"
#include <utility>

#ifdef ZE_GAME
#include <REngine.h>
#include <RResourceCache.h>
#include <RBuffer.h>
#include <RPipelineState.h>
#include <RDevice.h>
#include <RStateMachine.h>
#include <RVertexShader.h>
#include <RPixelShader.h>
#include <RInputLayout.h>
#include <RTools.h>
#include <RTexture.h>

#include "renderer/vertextypes.h"

extern RAPI::RBuffer* MakeBox(float extends);
extern RAPI::RBuffer* loadZENMesh(const std::string& file, float scale, std::vector<Math::float3>& zenVertices, std::vector<uint32_t>& zenIndices);
#endif

#define OBJECT_HANDLE_COUNT_BITS 15
#define OBJECT_HANDLE_HANDLE_BITS (32 - OBJECT_HANDLE_COUNT_BITS)

template<class Func, class Tuple, size_t...Is>
void for_each_in_tuple(Func f, Tuple&& tuple, std::index_sequence<Is...>){
    using expander = int[];
    (void)expander { 0, ((void)f(std::get<Is>(std::forward<Tuple>(tuple))), 0)... };
}

template<class Func, class Tuple>
void for_each_in_tuple(Func f, Tuple&& tuple){
    for_each_in_tuple(f, std::forward<Tuple>(tuple),
               std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>());
}

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
        Entity(uint32_t offset = 0) :
        mask(C_NONE)
    {
        handle.count = 0;
        handle.offset = offset;
    }


        ObjectHandle handle;
        EComponents mask;
    };

    class Engine;

    /**
     * @brief The ObjectFactory class creates and stores entities
     */
    template<typename... C>
    class ObjectFactory
    {
        friend class Engine;
    public:
        ObjectFactory(Engine *pEngine, uint32_t initialSize = 0) :
            m_pEngine(pEngine),
            m_Entities(initialSize)
        {
        }

        ~ObjectFactory()
        {
        }

        ObjectHandle createEntity()
        {
            uint32_t offset = firstFreeEntityOffset();
            ObjectHandle handle;
            if(offset == static_cast<uint32_t>(-1))
            {
                handle.handle = 0;
                return handle;
            }

            handle.offset = offset;
            handle.count = ++m_Entities[handle.offset].handle.count;
            return handle;
        }

        void destroyEntity(uint32_t offset)
        {
             if(m_Entities.size() <= offset)
                return;

            Entity &entity = m_Entities[offset];

//            for_each_in_tuple(m_Components, [&](auto t)
//            {
//                if((entity.mask & decltype(t)::value_type::MASK) == decltype(t)::value_type::MASK)
//                    std::get<decltype(t)>(m_Components)[offset].cleanUp();
//            });

            entity.mask = C_NONE;
        }

        void destroyEntity(ObjectHandle handle)
        {
            if(m_Entities.size() <= handle.offset)
                return;

            destroyEntity(handle.offset);
        }

        void test_createObjects();

        void test_createPhysicsEntity(const Math::float3& position, const Math::float3& impulse);

        Utils::Vector<Entity> &getEntities()
        {
            return m_Entities;
        }

        template<typename S>
        S *getComponent(const ObjectHandle &handle)
        {
            if(handle.offset >= m_Entities.size() || m_Entities[handle.offset].handle.count != handle.count)
                return nullptr;

            return &std::get<Utils::Vector<S>>(m_Components)[handle.offset];
        }

        template<typename S>
        S *addComponent(const ObjectHandle &handle)
        {
            if(handle.offset >= m_Entities.size())
                return nullptr;

            Entity &entity = m_Entities[handle.offset];
            if(m_Entities[handle.offset].handle.count != handle.count)
                return nullptr;

            if((entity.mask & S::value_type::MASK) == S::value_type::MASK)
                return nullptr;

            entity.mask = static_cast<EComponents>(entity.mask | S::value_type::MASK);
            return &std::get<Utils::Vector<S>>(m_Components)[handle.offset];
        }

        template<typename S>
        bool removeComponent(const ObjectHandle &handle)
        {
            Entity &entity = m_Entities[handle.offset];
            if(handle.offset >= m_Entities.size() || entity.handle.count != handle.count)
                return false;

            if((entity.mask & S::value_type::MASK) != S::value_type::MASK)
                return false;

            std::get<Utils::Vector<S>>(m_Components)[handle.offset].cleanUp();
            entity.mask = static_cast<EComponents>(entity.mask | ~S::value_type::MASK);

            return true;
        }

        void cleanUp()
        {
            for(uint32_t i = 0, end = m_Entities.size(); i < end; ++i)
                destroyEntity(m_Entities[i].handle);
        }

    private:
        /**
         * @brief
         * @return object offset
         */
        uint32_t firstFreeEntityOffset()
        {
            uint32_t offset, end;
            for(offset = 0, end = m_Entities.size(); offset < end; ++offset)
            {
                if(m_Entities.m_Data[offset].mask == C_NONE)
                    return offset;
            }

            if(m_Entities.size() >= Math::ipow(2, OBJECT_HANDLE_HANDLE_BITS))
                return static_cast<uint32_t>(-1);

            m_Entities.m_Data.emplace_back(m_Entities.size());
            //TODO
            return offset;
        }

        /**
         * @brief a pointer to the engine object
         */
        Engine *m_pEngine;

        /**
         * @brief this vector stores all entity objects
         */
        Utils::Vector<Entity> m_Entities;

        /**
         * @brief this tuple stores all component vectors (fucking awesome :D)
         */
        std::tuple<Utils::Vector<C>...> m_Components;
    };
}
