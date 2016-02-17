#pragma once 
#include <vector>
#include "utils/mathlib.h"
#include "zTypes.h"

namespace ZenConvert
{
	class Parser;
	class zCMesh
	{
	public:
		/**
		 * @brief Reads the mesh-object from the given binary stream
		 */
		void readObjectData(Parser& parser);

		/**
		@ brief returns the vector of vertex-positions
		*/
		const std::vector<Math::float3>& getVertices() { return m_Vertices; }

		/**
		@ brief returns the vector of features
		*/
		const std::vector<zTMSH_FeatureChunk>& getFeatures() { return m_Features; }


		/**
		@ brief returns the vector of triangle-indices
		*/
		const std::vector<uint32_t>& getIndices() { return m_Indices; }

		/**
		@ brief returns the vector of triangle-indices
		*/
		const std::vector<uint32_t>& getFeatureIndices() { return m_FeatureIndices; }

	private:

		/**
		 * @brief vector of vertex-positions for this mesh
		 */
		std::vector<Math::float3> m_Vertices;

		/** 
		 * @brief Featues for the vertices with the corresponding index
		 */
		std::vector<zTMSH_FeatureChunk> m_Features;

		/**
		* @brief indices for the triangles of the mesh
		*/
		std::vector<uint32_t> m_Indices;

		/**
		* @brief indices for the triangles of the mesh
		*/
		std::vector<uint32_t> m_FeatureIndices;
	};
}