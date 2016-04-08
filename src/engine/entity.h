#pragma once

#include "components.h"
#include "utils/mathlib.h"

namespace Engine
{
	/**
	 * @brief Single game object. Can have multiple components, accessed by using the same
	 *		  handle and by checking the mask.
	 */
    class Entity
    {
	public:
        Entity(uint32_t offset = 0)
		{
			m_ComponentMask = EComponents::C_NONE;
			m_Handle.count = 0;
			m_Handle.offset = offset;
		}

		/**
		 * Simple getters/setters
		 */
		void setHandle(const ObjectHandle& h) { m_Handle = h;}
		const ObjectHandle& getHandle() const { return m_Handle; }

		void setComponentMask(const EComponents& h) { m_ComponentMask = h;}
		const EComponents& getComponentMask() const { return m_ComponentMask; }

		void setWorldTransform(const Math::Matrix& h) { m_WorldTransform = h;}
		const Math::Matrix& getWorldTransform() const { return m_WorldTransform; }

		/**
		 * Attachments
		 */
		void addAttachment(ObjectHandle handle) { m_Attachments.insert(handle); }
		void removeAttachment(ObjectHandle handle) { m_Attachments.erase(handle); }
		const std::set<ObjectHandle>& getAttachments() { return m_Attachments; }

	protected:
		/**
		 * @brief Handle for access to the components stored along this entity
		 */
        ObjectHandle m_Handle;

		/**
		 * @brief Valid components for this Entity ORed together
		 */
        EComponents m_ComponentMask;

		/**
		 * @brief World-Transform of this entity
		 */
		Math::Matrix m_WorldTransform;

		/**
		 * @brief List of entities attached to this one in any way
		 */
		std::set<ObjectHandle> m_Attachments;
    };
}

