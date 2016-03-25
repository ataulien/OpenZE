#pragma once 
#include <vector>
#include <string>
#include "utils/mathlib.h"
#include "vdfs/fileIndex.h"
#include "engine/objectfactory.h"
#include "zenconvert/oCWorld.h"
#include "worldMesh.h"

const float DEFAULT_ZEN_SCALE_FACTOR = 1.0f / 100.0f;

namespace ZenConvert
{
	class zCMesh;
	class ZenParser;
}

namespace Renderer
{
	class Visual;
}

namespace Engine
{


	class ZenWorldMesh;
	class ZenWorld
	{
	public:
		ZenWorld(::Engine::Engine& engine, const std::string& zenFile, VDFS::FileIndex& vdfs, float scale = DEFAULT_ZEN_SCALE_FACTOR);
		~ZenWorld();

		/**
		 * @brief Loads a ZEN-File from the given parser and VDFS-Information
		 */
		void loadWorld(::Engine::Engine& engine, ZenConvert::ZenParser& parser, VDFS::FileIndex & vdfs, float scale = DEFAULT_ZEN_SCALE_FACTOR);

		void render(const Math::Matrix& viewProj);

		/**
		 * @brief Creates the visual for the given file. Uses one from cache if it already exists
		 */
		Renderer::Visual* loadVisual(const std::string& visual, float scale = DEFAULT_ZEN_SCALE_FACTOR);

		/**
		* @brief Returns the world-mesh
		*/
		WorldMesh& getWorldMesh(){ return m_WorldMesh; }

		/**
		 * @brief Updates the ground-polygon information for the given entity
		 */
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

		/**
		 * @brief Main engine
		 */
		::Engine::Engine* m_pEngine;

		/** 
		 * @brief Representation of the main-worldmesh
		 */
		WorldMesh m_WorldMesh;
	};
}
