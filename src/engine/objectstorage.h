#pragma once

#include <utility>
#include <tuple>

#include "utils/vector.h"
#include "objecthandle.h"
#include "entity.h"
#include "utils/tuple.h"

#ifdef ZE_GAME
#include "components/visual.h"
#endif

#include "components/collision.h"
#include "components/movement.h"

namespace Engine
{
	/**
	 * @brief Allocator and storage for game entities and components.
	 * @param C... List of component-types which are to be created using this class
	 */
    template<typename... C>
    class ObjectStorage
    {
    public:
        ObjectStorage(uint32_t initialSize)
        {
			// Pre-Allocate some space so we don't waste time on allocation
			// when loading a world for example
            Utils::for_each_in_tuple(m_Components, [&](auto &v)
            {
                v.resize(initialSize);
            });

			m_FreeEntityAvailable = false;
        }

		/**
		 * @brief Creates a new entity and returns the handle for access
		 */
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

			ObjectHandle h = m_Entities[handle.offset].getHandle();
			h.count++;

			m_Entities[handle.offset].setHandle(h);

            handle.count = m_Entities[handle.offset].getHandle().count;
            return handle;
        }

		/**
		 * @brief Removes an entity and cleans the registered conponents
		 */
        void destroyEntity(uint32_t offset)
        {
            if(m_Entities.size() <= offset)
                return;

            Entity &entity = m_Entities[offset];

            Utils::for_each_in_tuple(m_Components, [&](auto &v)
            {
                if((entity.getComponentMask() & std::remove_reference<decltype(v)>::type::value_type::MASK) == std::remove_reference<decltype(v)>::type::value_type::MASK)
                    std::get<typename std::remove_reference<decltype(v)>::type>(m_Components)[offset].cleanUp();
            });

            entity.setComponentMask(C_NONE);

			m_FreeEntityAvailable = true;
        }

		/**
		 * @brief Removes an entity and cleans the registered conponents
		 */
        void destroyEntity(ObjectHandle handle)
        {
            if(m_Entities.size() <= handle.offset)
                return;

            destroyEntity(handle.offset);
        }

		/**
		 * @brief Removes a component registered for an entity
		 */
        template<typename S>
        bool removeComponent(const ObjectHandle &handle)
        {
            Entity &entity = m_Entities[handle.offset];
            if(handle.offset >= m_Entities.size() || entity.getHandle().count != handle.count)
                return false;

            if((entity.getComponentMask() & S::MASK) != S::MASK)
                return false;

            std::get<Utils::Vector<S>>(m_Components)[handle.offset].cleanUp();
            entity.setComponentMask(static_cast<EComponents>(entity.getComponentMask() | ~S::MASK));

            return true;
        }

		/** 
		 * @brief Adds a new component of the type S to the given entity
		 */
        template<typename S>
        S *addComponent(const ObjectHandle &handle)
        {
            if(handle.offset >= m_Entities.size())
                return nullptr;

            Entity &entity = m_Entities[handle.offset];
            if(m_Entities[handle.offset].getHandle().count != handle.count)
                return nullptr;

            if((entity.getComponentMask() & S::MASK) == S::MASK)
                return nullptr;

            entity.setComponentMask(static_cast<EComponents>(entity.getComponentMask() | S::MASK));
            return &std::get<Utils::Vector<S>>(m_Components)[handle.offset];
        }

		/** 
		 * @brief Returns all entities created by this object
		 */
        Utils::Vector<Entity> &getEntities()
        {
            return m_Entities;
        }

		/**
		 * @brief returns the entity using the given handle
		 */
		Entity* getEntity(const ObjectHandle &handle)
		{
			if(handle.offset >= m_Entities.size())
				return nullptr;

			return &m_Entities[handle.offset];
		}

		/** 
		 * @brief returns the component of type S for the given entity-handle
		 */
        template<typename S>
        S *getComponent(const ObjectHandle &handle)
        {
            if(handle.offset >= m_Entities.size() || m_Entities[handle.offset].getHandle().count != handle.count)
                return nullptr;

            return &std::get<Utils::Vector<S>>(m_Components)[handle.offset];
        }

		/**
		 * @brief Destroys all created entities and their components
		 */
        void cleanUp()
        {
            for(uint32_t i = 0, end = m_Entities.size(); i < end; ++i)
                destroyEntity(m_Entities[i].getHandle());
        }

    private:
		/**
		 * @brief calculates the offset to the first free entity
		 */
        uint32_t firstFreeEntityOffset()
        {
            uint32_t offset, end;

			// Check if there is a free one in the list
			// TODO: Make this use a linked list to speed up entity-creation!
			if(m_FreeEntityAvailable)
			{
				for(offset = 0, end = m_Entities.size(); offset < end; ++offset)
				{
					if(m_Entities.m_Data[offset].getComponentMask() == C_NONE)
						return offset;
				}
			}

			// Are we full?
            if(m_Entities.size() >= Math::ipow(2, OBJECT_HANDLE_HANDLE_BITS))
                return static_cast<uint32_t>(-1);

			m_FreeEntityAvailable = false;

			// Create a new entity
            m_Entities.m_Data.emplace_back(m_Entities.size());
			m_Entities.m_Data.back().setComponentMask(C_IN_USE);

			// Add a slot for each supported component, ready to be used by the entity
            Utils::for_each_in_tuple(m_Components, [&](auto &v)
            {
                v.m_Data.emplace_back();
            });

            return m_Entities.size() - 1;
        }

		/**
		 * @brief List of all created entites. This list can contain unused (empty) entities,
		 *		  which can be identified by their component mask having the C_IN_USE-bit not set.
		 */
        Utils::Vector<Entity> m_Entities;

		/**
		 * @brief Whether there is at least one free entity insite m_Entities. 
		 *		  This is only needed to speed loading because then there won't be any free ones!
		 */
		bool m_FreeEntityAvailable;

		/**
		 * @brief vectors of components for each type we are managing here.
		 *		  All vectors are of the same size and components using the same
		 *		  index in these vectors are connected to the same entity
		 */
        std::tuple<Utils::Vector<C>...> m_Components;
    };
}

