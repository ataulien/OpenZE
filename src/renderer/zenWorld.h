#pragma once 
#include <vector>
#include <string>
#include "utils/mathlib.h"
#include "vdfs/fileIndex.h"

namespace Renderer
{
	class ZenWorldMesh;
	class ZenWorld
	{
	public:
		ZenWorld(const std::string& zenFile);
		ZenWorld(const std::string& zenFile, VDFS::FileIndex& vdfs);
		~ZenWorld();

		void render(const Math::Matrix& viewProj);
	private:
		std::vector<Renderer::ZenWorldMesh*> m_Meshes;
		std::vector<Math::float3> m_VobPositions;
	};
}
