#include "renderSystem.h"
#include "visuals/staticMeshVisual.h"
#include "visuals/skeletalMeshVisual.h"
#include "engine/game/gameengine.h"
#include "utils/tuple.h"
#include <RPipelineState.h>
#include <REngine.h>
#include <RDevice.h>

using namespace Renderer;

RenderSystem::RenderSystem(Engine::GameEngine& engine)
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

void Renderer::RenderSystem::renderFrame(float deltaTime)
{
	ZenConvert::SkeletalMeshInstanceInfo tmpSkelInstance;

	// Draw frame
	RAPI::REngine::RenderingDevice->OnFrameStart();
	RAPI::RRenderQueueID queueID = RAPI::REngine::RenderingDevice->AcquireRenderQueue(true, "Main Queue");


	Utils::Vector<Engine::Entity> &entities = m_pEngine->objectFactory().storage().getEntities();
	static std::vector<std::pair<std::vector<Engine::ObjectHandle>, std::vector<Renderer::PerInstanceData>>> piDataPerVisual;
	piDataPerVisual.resize(getVisualStorage().getVisuals().size());

	for(uint32_t i = 0, end = entities.size(); i < end; ++i)
		//for(uint32_t i = 0, end = 5; i < end; ++i)
	{
		Engine::Entity &entity = entities[i];

		if((entity.getComponentMask() & Engine::C_VISUAL) == 0)
			continue;

		Engine::Components::Visual *pVisual = m_pEngine->objectFactory().storage().getComponent<Engine::Components::Visual>(entity.getHandle());

		if((entity.getComponentMask() & Engine::C_ANIM_CONTROLLER) != 0)
		{
			if(pVisual->visualSubId == 0)
			{
				Engine::Components::AnimationController* pAnim = m_pEngine->objectFactory().storage().getComponent<Engine::Components::AnimationController>(entity.getHandle());
				pAnim->animHandler.UpdateAnimations(deltaTime);
				//pAnim->animHandler.DebugDrawSkeleton(entity.getWorldTransform());

				// Copy everything to the temporary skeletal instance
				pAnim->animHandler.UpdateSkeletalMeshInfo(tmpSkelInstance);
				tmpSkelInstance.color = pVisual->colorMod;
				tmpSkelInstance.worldMatrix = entity.getWorldTransform();
				pVisual->pObjectBuffer->UpdateData(&tmpSkelInstance);
			}

			RAPI::REngine::RenderingDevice->QueuePipelineState(pVisual->pPipelineState, queueID);

			continue;
		}


		if(entity.getWorldTransform().Translation() != Math::float3(0,0,0) && (m_pEngine->getCameraCenter() - entity.getWorldTransform().Translation()).lengthSquared() > 50.0f * 50.0f)
			continue;

		Math::Matrix modelViewProj;
		if((entity.getComponentMask() & Engine::C_COLLISION) == Engine::C_COLLISION)
		{
			Engine::Components::Collision *pCollision = m_pEngine->objectFactory().storage().getComponent<Engine::Components::Collision>(entity.getHandle());
			if(pCollision)
			{
				Math::Matrix model;
				static_cast<Physics::MotionState*>(pCollision->rigidBody.getMotionState())->openGLMatrix(reinterpret_cast<float*>(&model));

				if(entity.getWorldTransform() != model)
				{
					ZenConvert::VobObjectInfo ocb;
					ocb.color = Math::float4(1.0f, 1.0f, 1.0f, 1.0f);
					ocb.worldMatrix = model;
					pVisual->pObjectBuffer->UpdateData(&ocb);

					entity.setWorldTransform(model);
				}

			}
			else
				LogWarn() << "Invalid collision object";
		}

		Renderer::PerInstanceData pid;
		pid.WorldMatrix = entity.getWorldTransform();
		pid.Color = pVisual->colorMod;


		if(piDataPerVisual[pVisual->visualId].first.size() < pVisual->visualSubId + 1)
			piDataPerVisual[pVisual->visualId].first.resize(pVisual->visualSubId + 1);

		if(!piDataPerVisual[pVisual->visualId].first[pVisual->visualSubId].handle)
			piDataPerVisual[pVisual->visualId].first[pVisual->visualSubId] = entity.getHandle();

		if(pVisual->visualSubId == 0)
			piDataPerVisual[pVisual->visualId].second.push_back(pid);
		//RAPI::REngine::RenderingDevice->QueuePipelineState(pVisual->pPipelineState, queueID);
	}

	// Update and draw instances
	for(auto& inst : piDataPerVisual)
	{
		if(inst.second.empty())
			continue;

		Engine::Components::Visual *pVisual = m_pEngine->objectFactory().storage().getComponent<Engine::Components::Visual>(inst.first[0]);

		pVisual->pInstanceBuffer->UpdateData(inst.second.data(), inst.second.size() * sizeof(Renderer::PerInstanceData));

		for(const auto& ent : inst.first)
		{
			pVisual = m_pEngine->objectFactory().storage().getComponent<Engine::Components::Visual>(ent);
			pVisual->pPipelineState->NumInstances = inst.second.size();
			RAPI::REngine::RenderingDevice->QueuePipelineState(pVisual->pPipelineState, queueID);
		}

		inst.second.clear();
	}

	RAPI::REngine::RenderingDevice->ProcessRenderQueue(queueID);

	// Process frame
	RAPI::REngine::RenderingDevice->OnFrameEnd();

	RAPI::REngine::RenderingDevice->Present();
}

/**
* @brief Creates a visual for the given type in the target function overload
*/
StaticMeshVisual* RenderSystem::createVisual(size_t hash, const ZenConvert::PackedMesh& packedMesh)
{
	// Check if this was already loaded
	Visual* pCached = getVisualByHash(hash); // FIXME: This should be typesafe!
	if(pCached)
		return reinterpret_cast<StaticMeshVisual*>(pCached); 

	// Create a static mesh visual for this
	StaticMeshVisual* pVisual = new StaticMeshVisual(*this, m_pEngine->objectFactory());
	m_VisualStorage.addVisual(hash, pVisual); // Let the storage free the memory for now
											  // TODO: Handle unloading of visuals

	pVisual->createMesh(packedMesh);

	return pVisual;
}

/**
* @brief Creates a visual for the given type in the target function overload
*/
SkeletalMeshVisual* RenderSystem::createVisual(size_t hash, const ZenConvert::PackedSkeletalMesh& packedMesh)
{
	// Check if this was already loaded
	Visual* pCached = getVisualByHash(hash); // FIXME: This should be typesafe!
	if(pCached)
		return reinterpret_cast<SkeletalMeshVisual*>(pCached); ;

	// Create a static mesh visual for this
	SkeletalMeshVisual* pVisual = new SkeletalMeshVisual(*this, m_pEngine->objectFactory());
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
size_t RenderSystem::addEntityForVisual(Engine::ObjectHandle entityHandle, size_t visualId, size_t subVisualId)
{
	if(m_VisualInstanceCache.size() <= visualId + 1)
		m_VisualInstanceCache.resize(visualId + 1);

	Engine::Entity& entity = *m_pEngine->objectFactory().storage().getEntity(entityHandle);
	Engine::Components::Visual* pVisual = m_pEngine->objectFactory().storage().getComponent<Engine::Components::Visual>(entityHandle);

	PerInstanceData d;
	d.WorldMatrix = entity.getWorldTransform();

	// Verify the sub-vectors size
	if(m_VisualInstanceCache[visualId].size() <= subVisualId + 1)
		m_VisualInstanceCache[visualId].resize(subVisualId + 1);

	if(m_VisualInstanceCache[visualId][subVisualId].instanceDataPerSubIdx.empty())
		m_VisualInstanceCache[visualId][subVisualId].mainHandle = entityHandle;

	

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

					// Precompute and store the commandstream
					// pVisual->pPipelineState->PrecomputeCommandStream();
					// memcpy(pVisual->precomputedCommandStream, pVisual->pPipelineState->PrecomputedCommandStream, sizeof(pVisual->precomputedCommandStream));
					// pVisual->precomputedCommandStreamSize = pVisual->pPipelineState->PrecomputedCommandStreamSize;
				}

				inst += m_VisualInstanceCache[i][j].instanceDataPerSubIdx.size();
				data += size;
			}
		}
	}
	LEB(targetBuffer->Unmap());

}