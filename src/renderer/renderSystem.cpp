#include "renderSystem.h"
#include "visuals/staticMeshVisual.h"
#include "engine/engine.h"
#include "utils/tuple.h"
#include <RPipelineState.h>

using namespace Renderer;

RenderSystem::RenderSystem(Engine::Engine& engine)
{
	m_pEngine = &engine;

	clearInstanceCache();

	// Create main instancing buffer
	m_pInstancingBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	m_pInstancingBuffer->Init(nullptr, sizeof(PerInstanceData), sizeof(PerInstanceData), RAPI::B_VERTEXBUFFER, RAPI::U_DYNAMIC, RAPI::CA_WRITE);
	RAPI::REngine::ResourceCache->AddToCache("MainInstancingBuffer", m_pInstancingBuffer);
}

RenderSystem::~RenderSystem()
{
	RAPI::REngine::ResourceCache->DeleteResource(m_pInstancingBuffer);

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

/**
 * @brief clears the instance cache vector
 */
void RenderSystem::clearInstanceCache()
{
	for(auto& v : m_VisualInstanceCache)
	{
		for(auto& s : v)
			s.instanceDataPerSubIdx.clear();
	}

	m_NumRegisteredInstances = 0;
}

/**
 * @brief Adds an entity for the given visual
 */
size_t RenderSystem::addEntityForVisual(Engine::ObjectHandle entity, size_t visualId, size_t subVisualId)
{
	if(visualId > 306)
		return 0;

	if(m_VisualInstanceCache.size() <= visualId + 1)
		m_VisualInstanceCache.resize(visualId + 1);

	Engine::Components::Visual* pVisual = m_pEngine->objectFactory().storage().getComponent<Engine::Components::Visual>(entity);

	PerInstanceData d;
	d.WorldMatrix = pVisual->tmpWorld;

	// Verify the sub-vectors size
	if(m_VisualInstanceCache[visualId].size() <= subVisualId + 1)
		m_VisualInstanceCache[visualId].resize(subVisualId + 1);

	if(m_VisualInstanceCache[visualId][subVisualId].instanceDataPerSubIdx.empty())
		m_VisualInstanceCache[visualId][subVisualId].mainHandle = entity;

	

	m_VisualInstanceCache[visualId][subVisualId].instanceDataPerSubIdx.push_back(d);
	m_NumRegisteredInstances++;

	return m_VisualInstanceCache[visualId][subVisualId].instanceDataPerSubIdx.size();
}

/**
 * @brief Builds instancing-data and stores it into the given RBuffer
 */
void RenderSystem::buildInstancingData(RAPI::RBuffer* targetBuffer, std::vector<Engine::ObjectHandle>& mainHandles)
{
	// Resize buffer if needed
	if(m_NumRegisteredInstances * sizeof(PerInstanceData) > targetBuffer->GetSizeInBytes())
		targetBuffer->UpdateData(nullptr, m_NumRegisteredInstances * sizeof(PerInstanceData));

	uint8_t* data;
	size_t inst = 0;
	LEB(targetBuffer->Map(reinterpret_cast<void**>(&data)));
	for(size_t i = 0, end = m_VisualInstanceCache.size(); i < end; i++)
	{
		for(size_t j = 0, end = m_VisualInstanceCache[i].size(); j < end; j++)
		{
			auto& instVec = m_VisualInstanceCache[i][j];

			if(!instVec.instanceDataPerSubIdx.empty())
			{
				// Probably do this in an other loop for better cache hits on the data-buffer?
				mainHandles.push_back(m_VisualInstanceCache[i][j].mainHandle);

				size_t size = m_VisualInstanceCache[i][j].instanceDataPerSubIdx.size() * sizeof(PerInstanceData);

				if(inst * sizeof(PerInstanceData) + size > targetBuffer->GetSizeInBytes())
					break; // TODO: Testcode, remove!

				memcpy(data, m_VisualInstanceCache[i][j].instanceDataPerSubIdx.data(), size);

				// TODO: This destroys cache...
				Engine::Components::Visual* pVisual = m_pEngine->objectFactory().storage().getComponent<Engine::Components::Visual>(m_VisualInstanceCache[i][j].mainHandle);
				if(pVisual)
				{
					pVisual->pPipelineState->StartInstanceOffset = inst;
					pVisual->pPipelineState->NumInstances = m_VisualInstanceCache[i][j].instanceDataPerSubIdx.size();
				}

				inst += m_VisualInstanceCache[i][j].instanceDataPerSubIdx.size();
				data += size;
			}
		}
	}
	LEB(targetBuffer->Unmap());

}