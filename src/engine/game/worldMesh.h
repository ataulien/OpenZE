#pragma once
#include "../../zenconvert/zTypes.h"

namespace Engine
{
	/** 
	 * @brief Storage for the main static world-mesh data, acceleration-structures for access and other
	 *		  depending objects
	 */
	class WorldMesh
	{
	public:
		WorldMesh(){}

		/**
		 * @brief Initializes the object
		 */
		void setMeshData(const ZenConvert::PackedMesh& mesh)
		{
			m_MeshData = mesh;
		}

		/**
		 * @brief returns the list of triangles in this worldmesh
		 */
		const std::vector<ZenConvert::WorldTriangle>& getTriangleList() const { return m_MeshData.triangles; }
	protected:

		/**
		 * @brief Copy of the full mesh-data associated with this object
		 */
		ZenConvert::PackedMesh m_MeshData;
	};
}