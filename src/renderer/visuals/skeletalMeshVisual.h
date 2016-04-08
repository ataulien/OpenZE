#pragma once
#include <RPagedBuffer.h>
#include "../vertextypes.h"
#include "zenconvert/zTypes.h"
#include "visual.h"
#include <vector>

namespace ZenConvert
{
	class zCModelMeshLib;
}

namespace Renderer
{
	class RenderSystem;
	class SkeletalMeshVisual : public Visual
	{
	public:
		SkeletalMeshVisual(RenderSystem& system, Engine::ObjectFactory& factory);
		~SkeletalMeshVisual();

		/**
		 * @brief Initializes the mesh with the given dataset. 
		 * Note: You need to flush the paged buffers in the rendersystem to move the data to the GPU.
		 */
		void createMesh(const ZenConvert::PackedSkeletalMesh& packedMesh);


		/**
		 * @brief Creates entities matching this visual
		 * TODO: Use entities already in the vector and only add to them, if the vector isn't empty
		 */
		virtual void createEntities(std::vector<Engine::ObjectHandle>& createdEntities) override;

	private:

		struct SubMesh
		{
			/**
			* @brief Logical index-buffer for this submesh
			*/
			RAPI::RLogicalBuffer<uint32_t>* indexBuffer;

			/**
			* @brief The material-information to use with this submesh
			* // TODO: Create material-class!
			*/
			ZenConvert::zCMaterialData material;

			/** 
			 * @brief Handles using this submesh
			 */
			// TODO: Object deletion
			std::set<Engine::ObjectHandle> submeshObjectHandles;
		};

		/**
		 * @brief Updates the created pipelinestates accordingly to the logical buffers
		 */
		void onLogicalVertexBuffersUpdated(void* userptr);
		void onLogicalIndexBuffersUpdated(void* userptr);

		/** 
		 * @brief All submeshes of this static-mesh
		 */
		std::vector<SubMesh> m_Submeshes;

		/**
		 * @brief Logical vertexbuffer for this mesh
		 */
		RAPI::RLogicalBuffer<SkeletalVertex>* m_pVertexBuffer;

	};
}