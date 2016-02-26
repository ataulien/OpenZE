#pragma once

#include <utility>
#include <tuple>

#include "utils/vector.h"
#include "objecthandle.h"
#include "entity.h"
#include "utils/tuple.h"

#include "components/collision.h"
#include "components/visual.h"

namespace Engine
{
    template<typename... C>
    class ObjectStorage
    {
    public:
        ObjectStorage(uint32_t initialSize)
        {
            Utils::for_each_in_tuple(m_Components, [&](auto &v)
            {
                v.resize(initialSize);
            });
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

            Utils::for_each_in_tuple(m_Components, [&](auto &v)
            {
                if((entity.mask & std::remove_reference<decltype(v)>::type::value_type::MASK) == std::remove_reference<decltype(v)>::type::value_type::MASK)
                    std::get<typename std::remove_reference<decltype(v)>::type>(m_Components)[offset].cleanUp();
            });

            entity.mask = C_NONE;
        }

        void destroyEntity(ObjectHandle handle)
        {
            if(m_Entities.size() <= handle.offset)
                return;

            destroyEntity(handle.offset);
        }

        template<typename S>
        bool removeComponent(const ObjectHandle &handle)
        {
            Entity &entity = m_Entities[handle.offset];
            if(handle.offset >= m_Entities.size() || entity.handle.count != handle.count)
                return false;

            if((entity.mask & S::MASK) != S::MASK)
                return false;

            std::get<Utils::Vector<S>>(m_Components)[handle.offset].cleanUp();
            entity.mask = static_cast<EComponents>(entity.mask | ~S::MASK);

            return true;
        }

        template<typename S>
        S *addComponent(const ObjectHandle &handle)
        {
            if(handle.offset >= m_Entities.size())
                return nullptr;

            Entity &entity = m_Entities[handle.offset];
            if(m_Entities[handle.offset].handle.count != handle.count)
                return nullptr;

            if((entity.mask & S::MASK) == S::MASK)
                return nullptr;

            entity.mask = static_cast<EComponents>(entity.mask | S::MASK);
            return &std::get<Utils::Vector<S>>(m_Components)[handle.offset];
        }

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

        void cleanUp()
        {
            for(uint32_t i = 0, end = m_Entities.size(); i < end; ++i)
                destroyEntity(m_Entities[i].handle);
        }

    private:
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

            Utils::for_each_in_tuple(m_Components, [&](auto &v)
            {
                v.m_Data.emplace_back();
            });

            return offset;
        }

        Utils::Vector<Entity> m_Entities;
        std::tuple<Utils::Vector<C>...> m_Components;
    };
}

