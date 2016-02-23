#include "zCMesh.h"
#include "parser.h"
#include "utils/logger.h"
#include "zTypes.h"
#include <string>
#include "vdfs/fileIndex.h"
#include "vob.h"
#include "zCMaterial.h"

using namespace ZenConvert;

// Types of chunks we will find in a zCMesh-Section
static const unsigned short	MSID_MESH = 0xB000;
static const unsigned short	MSID_BBOX3D = 0xB010;
static const unsigned short	MSID_MATLIST = 0xB020;
static const unsigned short	MSID_LIGHTMAPLIST = 0xB025;
static const unsigned short	MSID_LIGHTMAPLIST_SHARED = 0xB026;
static const unsigned short	MSID_VERTLIST = 0xB030;
static const unsigned short	MSID_FEATLIST = 0xB040;
static const unsigned short	MSID_POLYLIST = 0xB050;
static const unsigned short	MSID_MESH_END = 0xB060;

/**
* @brief Loads the mesh from the given VDF-Archive
*/
zCMesh::zCMesh(const std::string& fileName, VDFS::FileIndex& fileIndex)
{
	ZenConvert::Chunk parentVob("parent", "", 0);
	ZenConvert::zCMesh worldMesh;

	std::vector<uint8_t> data;
	fileIndex.getFileData(fileName, data);

	try
	{
		// Create parser from memory
		// FIXME: There is an internal copy of the data here. Optimize!
		ZenConvert::Parser parser(data, nullptr, nullptr);
		
		// .MSH-Files are just saved zCMeshes
		readObjectData(parser, false);
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
void zCMesh::readObjectData(Parser& parser, bool fromZen)
{
	// Information about the whole file we are reading here
	BinaryFileInfo fileInfo;

	// Information about a single chunk 
	BinaryChunkInfo chunkInfo;

	size_t binFileEnd; // Ending location of the binary file

	if(fromZen)
	{
		// Read information about the current file. Mainly size is important here.
		parser.readStructure(fileInfo);

		// Calculate ending location and thus, the filesize
		binFileEnd = parser.getSeek() + fileInfo.size;
	}
	else
	{
		binFileEnd = parser.getFileSize();
	}

	// Read chunks until we left the virtual binary file or got to the end-chunk
	// Each chunk starts with a header (BinaryChunkInfo) which gives information
	// about what to do and how long the chunk is
	bool doneReadingChunks = false;
	while(!doneReadingChunks && parser.getSeek() <= binFileEnd)
	{
		// Read chunk header and calculate position of next chunk
		parser.readStructure(chunkInfo);

		size_t chunkEnd = parser.getSeek() + chunkInfo.length;

		switch(chunkInfo.id)
		{
		case MSID_MESH:
		{
			// uint32 - version
			// zDate - Structure
			// \n terminated string for the name
			uint32_t version = parser.readBinaryWord();
			zDate date; parser.readStructure(date);
			std::string name = parser.readLine(false);
			(void)date;

			LogInfo() << "Reading mesh '" << name << "' (Version: " << version << ")";

			parser.setSeek(chunkEnd); // Skip chunk
		}
		break;

		case MSID_BBOX3D:
		{
			Math::float4 min, max;
			parser.readStructure(min);
			parser.readStructure(max);

			m_BBMin = Math::float3(min.x, min.y,min.z);
			m_BBMax = Math::float3(max.x, max.y,max.z);

			parser.setSeek(chunkEnd); // Skip chunk
		}
			break;

		case MSID_MATLIST:
			{
				// ZenArchive - Header
				// uint32_t - Num materials
				// For each material:
				//  - String - Name
				//  - zCMaterial-Chunk

				Parser::Header tmpHeader;
				parser.readHeader(&tmpHeader);

				// Read number of materials
				uint32_t numMaterials = parser.readBinaryDword();

				// Read every stored material
				for(uint32_t i = 0; i < numMaterials; i++)
				{
					zCMaterial mat;
					mat.readObjectData(parser);

					// Save into vector
					m_Materials.emplace_back(mat.getMaterialInfo());
				}

				parser.setSeek(chunkEnd); // Skip chunk
			}
			break;

		case MSID_LIGHTMAPLIST:
			parser.setSeek(chunkEnd); // Skip chunk
			break;

		case MSID_LIGHTMAPLIST_SHARED:
			parser.setSeek(chunkEnd); // Skip chunk
			break;

		case MSID_VERTLIST:
			{
				// uint32 - number of vertices
				// numVx float3s of vertexpositions

				// Read how many vertices we have in this chunk
				uint32_t numVertices = parser.readBinaryDword();
				m_Vertices.clear();
				m_Vertices.reserve(numVertices);

				// Read vertex data and emplace into m_Vertices
				parser.readMultipleStructures(numVertices, m_Vertices);

				// Flip x-coord to make up for right handedness
				//for(auto& v : m_Vertices)
				//	v.x = -v.x;
				
			}
			break;

		case MSID_FEATLIST:
			{
				// uint32 - number of features
				// zTMSH_FeatureChunk*num - features

				// Read how many feats we have
				uint32_t numFeats = parser.readBinaryDword();

				// Read features
				parser.readMultipleStructures(numFeats, m_Features);
			}
			break;

		case MSID_POLYLIST:
			{
				// uint32 - number of polygons
				struct polyData1
				{
					uint16_t	materialIndex;
					uint16_t	lightmapIndex;
					zTPlane		polyPlane;
					PolyFlags	flags;
					uint8_t		polyNumVertices;
				};

				struct polyData2 : public polyData1
				{
					struct Index
					{
						uint32_t VertexIndex;
						uint32_t FeatIndex;
					};

					Index indices[255];
				};

				// Read number of polys
				int numPolys = parser.readBinaryDword();

				// Read block of data
				std::vector<uint8_t> dataBlock;
				parser.readMultipleStructures(chunkInfo.length, dataBlock);

				uint8_t* blockPtr = dataBlock.data();

				// Iterate throuh every poly
				for(int i = 0; i < numPolys; i++)
				{
					polyData2* p = (polyData2 *)blockPtr;

					// TODO: Triangulate polygons!
					if(p->polyNumVertices == 3) 
					{
						// Write indices directly to a vector
						for(int v = 0; v < p->polyNumVertices; v++)
						{
							m_Indices.emplace_back(p->indices[v].VertexIndex);
							m_FeatureIndices.emplace_back(p->indices[v].FeatIndex);
						}

						// Save material index for the written triangle
						m_TriangleMaterialIndices.emplace_back(p->materialIndex);
					}
					else
					{
						// Triangulate a triangle-fan
						for (unsigned int i = 1; i < p->polyNumVertices - 1; i++)
						{
							m_Indices.emplace_back(p->indices[0].VertexIndex);
							m_Indices.emplace_back(p->indices[i + 1].VertexIndex);
							m_Indices.emplace_back(p->indices[i].VertexIndex);

							m_FeatureIndices.emplace_back(p->indices[0].FeatIndex);
							m_FeatureIndices.emplace_back(p->indices[i + 1].FeatIndex);
							m_FeatureIndices.emplace_back(p->indices[i].FeatIndex);

							// Save material index for the written triangle
							m_TriangleMaterialIndices.emplace_back(p->materialIndex);
						}
					}

					// Goto next polygon using this weird shit
					blockPtr += sizeof(polyData1) + sizeof(polyData2::Index) * p->polyNumVertices;
				}

				parser.setSeek(chunkEnd); // Skip chunk, there could be more data here which is never read
			}
			break;

		case MSID_MESH_END:
			doneReadingChunks = true;
			break;

		default:
			parser.setSeek(chunkEnd); // Skip chunk
		}
	}

	// Skip to possible next section of the underlaying file, in case there is more data we don't process
	parser.setSeek(binFileEnd);
}