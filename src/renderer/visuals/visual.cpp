#include "visual.h"

Renderer::Visual::Visual(RenderSystem& system, Engine::ObjectFactory& factory)
{
	m_pRenderSystem = &system;
	m_pObjectFactory = &factory;
}

Renderer::Visual::~Visual()
{
	// Clean the visuals we have created
	for(auto& v : m_ObjectHandles)
	{
		Engine::Components::Visual* pVisual = m_pObjectFactory->storage().getComponent<Engine::Components::Visual>(v);

		if(pVisual)
		{
			pVisual->cleanUp();
			m_pObjectFactory->storage().removeComponent<Engine::Components::Visual>(v);
		}		
	}
}
