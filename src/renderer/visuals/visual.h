#pragma once
#include "engine/components/visual.h"
#include "engine/objecthandle.h"
#include "engine/objectfactory.h"
#include <set>
#include <vector>

namespace Renderer
{
	/**
	 * @brief 0-based ID, save to be used in arrays of the same size as number of components
	 */
	typedef size_t VisualID;

	class RenderSystem;
	class Visual
	{
	public:
		Visual(RenderSystem& system, Engine::ObjectFactory& factory);
		virtual ~Visual();

		/**
		 * @brief Creates entities matching this visual
		 */
		virtual void createEntities(std::vector<Engine::ObjectHandle>& createdEntities) = 0;

		/**
		 * @brief Returns/Set the ID of this visual
		 */
		VisualID getId(){return m_Id;}
		void setId(VisualID id){m_Id = id;}

		/**
		 * @brief Returns the amount of entities created/registered for this visual
		 */
		size_t getNumEntities(){return m_NumEntities;}

	protected:
		/**
		 * @brief Source rendersystem
		 */
		RenderSystem* m_pRenderSystem;

		/**
		 * @brief Object handles using visuals created by this
		 */
		std::set<Engine::ObjectHandle> m_ObjectHandles;
		Engine::ObjectFactory* m_pObjectFactory;

		/**
		 * @brief ID of this visual. These are always minimalistic, which means they are reused after an object died
		 */
		VisualID m_Id;

		/**
		 * @brief Amount of entities created for this visual
		 */
		size_t m_NumEntities;
	};
}