#include "renderSystem.h"
#include "visuals/staticMeshVisual.h"
#include "engine/engine.h"
#include "utils/tuple.h"

using namespace Renderer;

RenderSystem::RenderSystem(Engine::Engine& engine)
{
	m_pEngine = &engine;
}

RenderSystem::~RenderSystem()
{
	Utils::for_each_in_tuple(m_PagedVertexBuffers.types, [](auto t){delete t;});
	Utils::for_each_in_tuple(m_PagedIndexBuffers.types, [](auto t){delete t;});
}

/**
* @brief Creates a visual for the given type in the target function overload
*/
Visual* RenderSystem::createVisual(size_t hash, const ZenConvert::PackedMesh& packedMesh)
{
	// Check if this was already loaded
	Visual* pCached = getVisualByHash(hash);
	if(pCached)
		return pCached;

	// Create a static mesh visual for this
	StaticMeshVisual* pVisual = new StaticMeshVisual(*this, m_pEngine->objectFactory());
	m_VisualStorage.addVisual(hash, pVisual); // Let the storage free the memory for now
											  // TODO: Handle unloading of visuals

	pVisual->createMesh(packedMesh);

	return pVisual;
}

/**
* @brief Returns the visual matching the given hash
*/
Visual* RenderSystem::getVisualByHash(size_t hash)
{
	return m_VisualStorage.getVisual(hash);
}