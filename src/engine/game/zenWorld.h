#pragma once 
#include <vector>
#include <string>
#include "utils/mathlib.h"
#include "vdfs/fileIndex.h"
#include "engine/objectfactory.h"
#include "zenconvert/oCWorld.h"

const float DEFAULT_ZEN_SCALE_FACTOR = 1.0f / 100.0f;

namespace ZenConvert
{
	class zCMesh;
	class ZenParser;
}

namespace Engine
{


	class ZenWorldMesh;
	class ZenWorld
	{
	public:
		ZenWorld(::Engine::Engine& engine, const std::string& zenFile, VDFS::FileIndex& vdfs, float scale = DEFAULT_ZEN_SCALE_FACTOR);
		ZenWorld(::Engine::Engine& engine, ZenConvert::ZenParser& parser, VDFS::FileIndex & vdfs, float scale = DEFAULT_ZEN_SCALE_FACTOR);
		~ZenWorld();

		void render(const Math::Matrix& viewProj);
	private:

		/**
		 * @brief Disects the worldmesh into its parts and creates the needed entities
		 */
		void disectWorldMesh(ZenConvert::zCMesh* mesh, ::Engine::Engine& engine, VDFS::FileIndex & vdfs, float scale);

		/**
		 * @brief Creates entities for the loaded oCWorld
		 */
		void parseWorldObjects(const ZenConvert::oCWorldData& data, ::Engine::Engine& engine, VDFS::FileIndex & vdfs, float scale);

		std::vector<Math::float3> m_VobPositions;

	};
}
