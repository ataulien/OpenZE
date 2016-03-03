#pragma once
#include "engine/components/visual.h"
#include "engine/objecthandle.h"
#include "engine/objectfactory.h"
#include <set>
#include <vector>

namespace Renderer
{
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
		size_t getId(){return m_Id;}
		void setId(size_t id){m_Id = id;}

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
		size_t m_Id;
	};
}