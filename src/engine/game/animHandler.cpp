#include "animHandler.h"
#include "utils/logger.h"

#ifdef ZE_GAME
#include <RTools.h>
#endif

using namespace Engine;

AnimHandler::AnimHandler()
{
	m_ActiveAnimation = nullptr;
	m_AnimRootVelocity = Math::float3(0,0,0);
}

/**
 * @brief Sets the mesh-lib this operates on. Does a copy inside, so the given object can be deleted.
 */
void AnimHandler::SetMeshLib(const ZenConvert::zCModelMeshLib& meshLib)
{
	m_MeshLib = meshLib;

	m_NodeTransforms.resize(m_MeshLib.getNodes().size());
	m_ObjectSpaceNodeTransforms.resize(m_MeshLib.getNodes().size());
}

/**
 * @brief Adds an animation to the library
 */
void AnimHandler::AddAnimation(const ZenConvert::zCModelAni& ani)
{
	m_Animations[ani.getModelAniHeader().aniName] = ani;
}

/**
* @brief Sets the currently playing animation
*/
void AnimHandler::PlayAnimation(const std::string & animName)
{
	if(m_ActiveAnimation && m_ActiveAnimation->getModelAniHeader().aniName == animName)
		return; // TODO: Make extra function for that or something else that can query/update the current animation

	// Reset velocity
	m_AnimRootVelocity = Math::float3(0,0,0);

	// FIXME: No animation at all doesn't actually work, mesh gets scrambled
	if(animName.empty())
	{
		m_ActiveAnimation = nullptr;

		// Restore matrices from the bind-pose
		// because the animation won't modify all of the nodes
		for(size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
		{
			m_NodeTransforms[i] = m_MeshLib.getNodes()[i].transformLocal;
		}

		return;
	}

	// find and apply given animation name
	auto it = m_Animations.find(animName);
	if(it == m_Animations.end())
	{
		LogError() << "Failed to find animation: " << animName;
		m_ActiveAnimation = nullptr;
	}
	else
	{
		m_ActiveAnimation = &(*it).second;
		m_AnimationFrame = 0.0f;

		// Restore matrices from the bind-pose
		// because the animation won't modify all of the nodes
		for(size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
		{
			m_NodeTransforms[i] = m_MeshLib.getNodes()[i].transformLocal;
		}
	}
}

/**
* @brief Updates the currently playing animations
*/
void AnimHandler::UpdateAnimations(double deltaTime)
{
	// TODO: If not playing an animation, only initialize m_ObjectSpaceNodeTransforms once!
	if(m_ActiveAnimation)
	{

		for(size_t i = 0; i < m_ActiveAnimation->getNodeIndexList().size(); i++)
		{
			// TODO: Lerp between this and the next frame
			size_t frameNum = static_cast<size_t>(m_AnimationFrame);
			size_t numAnimationNodes = m_ActiveAnimation->getNodeIndexList().size();
			uint32_t nodeIdx = m_ActiveAnimation->getNodeIndexList()[i];

			// Extract sample at the current frame/node
			auto& sample = m_ActiveAnimation->getAniSamples()[frameNum * numAnimationNodes + i];

			// Build transformation matrix from the sample-information
			// Note: Precomputing this is hard because of interpolation
			Math::Matrix trans = Math::Matrix::CreateFromQuaternion(sample.rotation);
			trans.Translation(sample.position);

			m_NodeTransforms[nodeIdx] = trans;
		}
	}

	// Calculate actual node matrices
	for(size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
	{
		// TODO: There is a flag indicating whether the animation root should translate the vob position
		if(i==0)
			m_NodeTransforms[i].Translation(Math::float3(0.0f,0.0f,0.0f));

		if(m_MeshLib.getNodes()[i].parentValid())
			m_ObjectSpaceNodeTransforms[i] = m_ObjectSpaceNodeTransforms[m_MeshLib.getNodes()[i].parentIndex] * m_NodeTransforms[i];
		else 
			m_ObjectSpaceNodeTransforms[i] = m_NodeTransforms[i];
	}

	if(m_ActiveAnimation)
	{
		// Increase current timeline-position
		float framesPerSecond = m_ActiveAnimation->getModelAniHeader().fpsRate;
		float numFrames = m_ActiveAnimation->getModelAniHeader().numFrames;
		size_t lastFrame = static_cast<size_t>(m_AnimationFrame);
		m_AnimationFrame += deltaTime * framesPerSecond;
		if(m_AnimationFrame >= numFrames)
			m_AnimationFrame = 0.0f;

		size_t frameNum = static_cast<size_t>(m_AnimationFrame);

		// Get velocity of the current animation
		// FIXME: Need better handling of animation end
		if(lastFrame != frameNum && frameNum != 0)
		{
			// Get sample of root node (Node 0) from the current and the last frame
			auto& sampleCurrent = m_ActiveAnimation->getAniSamples()[frameNum * m_ActiveAnimation->getNodeIndexList().size()];
			auto& sampleLast = m_ActiveAnimation->getAniSamples()[lastFrame * m_ActiveAnimation->getNodeIndexList().size()];

			// Scale velocity to seconds
			m_AnimRootVelocity = (sampleCurrent.position - sampleLast.position) * m_ActiveAnimation->getModelAniHeader().fpsRate;
			//LogInfo() << "Samples " << lastFrame << " -> " << frameNum  << " = " << m_AnimRootVelocity.toString();
		}
	}
}

/**
* @brief Draws the skeleton of the animation
*/
void AnimHandler::DebugDrawSkeleton(const Math::Matrix& transform)
{
#ifdef ZE_GAME
	for(size_t i = 0; i < m_MeshLib.getNodes().size(); i++)
	{
		const ZenConvert::ModelNode& n  = m_MeshLib.getNodes()[i];
		const auto& t2 = m_ObjectSpaceNodeTransforms[i].Translation();

		Math::float3 p2 = transform * t2;

		RAPI::RTools::LineRenderer.AddPointLocator(p2.v, 0.04f);

		if(n.parentValid())
		{
			const auto& t1 = m_ObjectSpaceNodeTransforms[n.parentIndex].Translation();
			Math::float3 p1 = (transform * t1);

			RAPI::RTools::LineRenderer.AddLine(RAPI::LineVertex(p1.v, { 1.0f,1.0f,1.0f,1.0f }),
				RAPI::LineVertex(p2.v, { 1.0f,1.0f,1.0f,1.0f }));
		}
	}
#endif
}

/**
* @brief Updates the given skeletalmesh-info
*/
void AnimHandler::UpdateSkeletalMeshInfo(ZenConvert::SkeletalMeshInstanceInfo& info)
{
	memcpy(info.nodeTransforms, m_ObjectSpaceNodeTransforms.data(), std::min(m_NodeTransforms.size(), ZenConvert::MAX_NUM_SKELETAL_NODES) * sizeof(Math::Matrix));

	//static float s_test = 0;
	//s_test += 0.1f;
	//for(int i = 0; i < ZenConvert::MAX_NUM_SKELETAL_NODES; i++)
	//{
	//	info.nodeTransforms[i] = Math::Matrix::CreateTranslation(Math::float3(0, sinf(s_test), 0));
	//}
}