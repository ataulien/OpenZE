#pragma once
#include "zenconvert/zCModelAni.h"
#include "zenconvert/zCModelMeshLib.h"
#include <unordered_map>

namespace Engine
{
	class AnimHandler
	{
	public:

		AnimHandler();

		/**
		 * @brief Sets the mesh-lib this operates on. Does a copy inside, so the given object can be deleted.
		 */
		void SetMeshLib(const ZenConvert::zCModelMeshLib& meshLib);

		/**
		 * @brief Adds an animation to the library. Does a copy inside, so the given object can be deleted.
		 *		  TODO: Should not copy the animation-samples...
		 */
		void AddAnimation(const ZenConvert::zCModelAni& ani);

		/**
		 * @brief Sets the currently playing animation
		 */
		void PlayAnimation(const std::string& animName);

		/**
		 * @brief Updates the currently playing animations
		 */
		void UpdateAnimations(double deltaTime);

		/**
		 * @brief Draws the skeleton of the animation
		 */
		void DebugDrawSkeleton(const Math::Matrix& transform);

		/**
		 * @brief Updates the given skeletalmesh-info
		 */
		void UpdateSkeletalMeshInfo(ZenConvert::SkeletalMeshInstanceInfo& info);

		/**
		 * @return Velocity of the root node in m/s
		 */
		const Math::float3& getRootNodeVelocity(){ return m_AnimRootVelocity; }
	private:

		/**
		 * @brief Animations by their name
		 */
		std::unordered_map<std::string, ZenConvert::zCModelAni> m_Animations;

		/**
		 * @brief Meshlib this operates on
		 */
		ZenConvert::zCModelMeshLib m_MeshLib;

		/** 
		 * @brief Active animation
		 */
		const ZenConvert::zCModelAni* m_ActiveAnimation;
		float m_AnimationFrame;

		/** 
		 * @brief Node transforms in local space
		 */
		std::vector<Math::Matrix> m_NodeTransforms;

		/**
		 * @brief Node transforms in object-space
		 */
		std::vector<Math::Matrix> m_ObjectSpaceNodeTransforms;

		/**
		 * @brief Root-Node-Veclocity in m/s
		 */
		Math::float3 m_AnimRootVelocity;
	};
}