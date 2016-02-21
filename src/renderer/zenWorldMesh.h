#pragma once
#include <vector>
#include "utils/mathlib.h"
#include "vertextypes.h"

namespace RAPI
{
	struct RPipelineState;
	class RBuffer;
}

namespace ZenConvert
{
	class zCMesh;
}

namespace Renderer
{
	/**
	 * @brief Simple mesh container for meshes comming directly from a ZEN-File
	 */
	class ZenWorldMesh
	{
	public:
		ZenWorldMesh(const ZenConvert::zCMesh& source, float scale = 1.0f / 50.0f, const Math::float3& positionOffset = Math::float3(0,0,0));
		~ZenWorldMesh();

		struct SubMesh
		{
			RAPI::RPipelineState* state;
		};

		void render(const Math::Matrix& viewProj);

	private:
		std::vector<SubMesh> m_SubMeshes;
		std::vector<Renderer::WorldVertex> m_VerticesAsTriangles;
		std::unordered_map<std::string, RAPI::RBuffer*> m_BufferMap;
		RAPI::RBuffer* m_pObjectBuffer;
	};
}