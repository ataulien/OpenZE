#pragma once
#include <vector>
#include "utils/mathlib.h"
#include "vertextypes.h"
#include "zenconvert/export.h"
#include <REngine.h>

namespace RAPI
{
	struct RPipelineState;
	class RBuffer;
}

namespace ZenConvert
{
	class zCMesh;
	class zCProgMeshProto;
}

namespace VDFS
{
	class FileIndex;
}

namespace Renderer
{
	/**
	 * @brief Simple mesh container for meshes comming directly from a ZEN-File
	 */
	class ZenWorldMesh
	{
	public:
        ZenWorldMesh(const ZenConvert::zCMesh& source, VDFS::FileIndex& fileIndex, float scale = ZEN_SCALE_FACTOR, const Math::float3& positionOffset = Math::float3(0,0,0));
        ZenWorldMesh(const ZenConvert::zCProgMeshProto& source, VDFS::FileIndex& fileIndex, float scale = ZEN_SCALE_FACTOR, const Math::float3& positionOffset = Math::float3(0,0,0));
		~ZenWorldMesh();

		struct SubMesh
		{
			RAPI::RPipelineState* state;
		};

        void render(const Math::Matrix& viewProj, RAPI::RRenderQueueID queue);

	private:
		std::vector<SubMesh> m_SubMeshes;
		std::vector<Renderer::WorldVertex> m_VerticesAsTriangles;
		std::unordered_map<std::string, RAPI::RBuffer*> m_BufferMap;
		RAPI::RBuffer* m_pObjectBuffer;
	};
}
