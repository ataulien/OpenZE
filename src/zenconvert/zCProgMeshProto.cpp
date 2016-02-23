#include "zCProgMeshProto.h"
#include "parser.h"
#include "utils/logger.h"
#include "zTypes.h"
#include <string>
#include "vdfs/fileIndex.h"
#include "vob.h"
#include "zCMaterial.h"

using namespace ZenConvert;

static const uint16_t zCPROGMESH_FILE_VERS_G2 = 0x0905;
static const uint16_t MSID_PROGMESH = 0xB100;
static const uint16_t MSID_PROGMESH_END = 0xB1FF;

struct MeshDataEntry
{
	uint32_t offset;
	uint32_t size;
};

struct MeshOffsetsMain
{
	MeshDataEntry position;
	MeshDataEntry normal;
};

struct MeshOffsetsSubMesh
{
	MeshDataEntry triangleList;
	MeshDataEntry wedgeList;
	MeshDataEntry colorList;
	MeshDataEntry trianglePlaneIndexList;
	MeshDataEntry trianglePlaneList;
	MeshDataEntry wedgeMap;
	MeshDataEntry vertexUpdates;
	MeshDataEntry triangleEdgeList;
	MeshDataEntry edgeList;
	MeshDataEntry edgeScoreList;
};


/**
* @brief Loads the mesh from the given VDF-Archive
*/
zCProgMeshProto::zCProgMeshProto(const std::string& fileName, VDFS::FileIndex& fileIndex)
{

	std::vector<uint8_t> data;
	fileIndex.getFileData(fileName, data);

	try
	{
		// Create parser from memory
		// FIXME: There is an internal copy of the data here. Optimize!
		ZenConvert::Parser parser(data, nullptr, nullptr);
		
		readObjectData(parser);
	}
	catch(std::exception &e)
	{
		LogError() << e.what();
		return;
	}
}

/**
* @brief Reads the mesh-object from the given binary stream
*/
void zCProgMeshProto::readObjectData(Parser& parser)
{
	// Information about a single chunk 
	BinaryChunkInfo chunkInfo;

	bool doneReadingChunks = false;
	while(!doneReadingChunks && parser.getSeek() <= parser.getFileSize())
	{
		// Read chunk header and calculate position of next chunk
		parser.readStructure(chunkInfo);

		size_t chunkEnd = parser.getSeek() + chunkInfo.length;
		switch(chunkInfo.id)
		{
		case MSID_PROGMESH:
			{
				uint16_t version = parser.readBinaryWord();
				/*if(version != zCPROGMESH_FILE_VERS_G2)
				{
					LogWarn() << "Unsupported zCProgMeshProto-Version: " << version;
				}*/

				// Read data-pool
				uint32_t dataSize = parser.readBinaryDword();
				std::vector<uint8_t> dataPool;
				parser.readMultipleStructures(dataSize, dataPool);

				// Read how many submeshes we got
				uint8_t numSubmeshes = parser.readBinaryByte();

				// Read data offsets for the main position/normal-list
				MeshOffsetsMain mainOffsets; parser.readStructure(mainOffsets);

				// Read offsets to indices data
				std::vector<MeshOffsetsSubMesh> subMeshOffsets;
				parser.readMultipleStructures(numSubmeshes, subMeshOffsets);

				// Read materials
				{
					// ZenArchive - Header
					// uint32_t - Num materials
					// For each material:
					//  - String - Name
					//  - zCMaterial-Chunk

					Parser::Header tmpHeader;
					parser.readHeader(&tmpHeader);

					// Read every stored material
					for(uint32_t i = 0; i < numSubmeshes; i++)
					{
						zCMaterial mat;
						mat.readObjectData(parser);

						// Save into vector
						m_Materials.emplace_back(mat.getMaterialInfo());
					}		
				}

				// Read whether we want to have alphatesting
				m_IsUsingAlphaTest = parser.readBinaryByte() != 0;
				
				// Read boundingbox
				Math::float4 min, max;
				parser.readStructure(min);
				parser.readStructure(max);

				m_BBMin = Math::float3(min.x, min.y,min.z);
				m_BBMax = Math::float3(max.x, max.y,max.z);

				// Extract data
				m_Vertices.resize(mainOffsets.position.size);
				m_Normals.resize(mainOffsets.normal.size);

				// Copy vertex-data
				memcpy(m_Vertices.data(), &dataPool[mainOffsets.position.offset], sizeof(Math::float3) * mainOffsets.position.size);
				memcpy(m_Normals.data(), &dataPool[mainOffsets.normal.offset], sizeof(Math::float3) * mainOffsets.normal.size);
			
				// Copy submesh-data
				m_SubMeshes.resize(numSubmeshes);
				for(uint32_t i = 0; i < numSubmeshes; i++)
				{
					auto& d = subMeshOffsets[i];

					m_SubMeshes[i].m_Material = m_Materials[i];
					m_SubMeshes[i].m_TriangleList.resize(d.triangleList.size);
					m_SubMeshes[i].m_WedgeList.resize(d.wedgeList.size);
					m_SubMeshes[i].m_ColorList.resize(d.colorList.size);
					m_SubMeshes[i].m_TrianglePlaneIndexList.resize(d.trianglePlaneIndexList.size);
					m_SubMeshes[i].m_TrianglePlaneList.resize(d.trianglePlaneList.size);
					m_SubMeshes[i].m_TriEdgeList.resize(d.triangleEdgeList.size);
					m_SubMeshes[i].m_EdgeList.resize(d.edgeList.size);
					m_SubMeshes[i].m_EdgeScoreList.resize(d.edgeScoreList.size);
					m_SubMeshes[i].m_WedgeMap.resize(d.wedgeMap.size);

					memcpy(m_SubMeshes[i].m_TriangleList.data(), &dataPool[d.triangleList.offset], sizeof(zTriangle) * d.triangleList.size);
					memcpy(m_SubMeshes[i].m_WedgeList.data(), &dataPool[d.wedgeList.offset], sizeof(zWedge) * d.wedgeList.size);
					memcpy(m_SubMeshes[i].m_ColorList.data(), &dataPool[d.colorList.offset], sizeof(float) * d.colorList.size);
					memcpy(m_SubMeshes[i].m_TrianglePlaneIndexList.data(), &dataPool[d.trianglePlaneIndexList.offset], sizeof(uint16_t) * d.trianglePlaneIndexList.size);
					memcpy(m_SubMeshes[i].m_TrianglePlaneList.data(), &dataPool[d.trianglePlaneList.offset], sizeof(zTPlane) * d.trianglePlaneList.size);
					memcpy(m_SubMeshes[i].m_TriEdgeList.data(), &dataPool[d.triangleEdgeList.offset], sizeof(zTriangleEdges) * d.triangleEdgeList.size);
					memcpy(m_SubMeshes[i].m_EdgeList.data(), &dataPool[d.edgeList.offset], sizeof(zEdge) * d.edgeList.size);
					memcpy(m_SubMeshes[i].m_EdgeScoreList.data(), &dataPool[d.edgeScoreList.offset], sizeof(float) * d.edgeScoreList.size);
					memcpy(m_SubMeshes[i].m_WedgeMap.data(), &dataPool[d.wedgeMap.offset], sizeof(uint16_t) * d.wedgeMap.size);

				}
			}
			break;

		case MSID_PROGMESH_END:
			doneReadingChunks = true;
			break;
		default:
			parser.setSeek(chunkEnd);
		}
	}
}