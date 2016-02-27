#pragma once
#include "zTypes.h"
#include "zenParser.h"
#include "zCVob.h"

namespace ZenConvert
{
	class oCWorld
	{
	public:

		static void readVobTree(ZenParser& parser, zCVobData& target)
		{

		}

		/**
		* Reads this object from an internal zen
		*/
		static oCWorldData readObjectData(ZenParser& parser)
		{
			oCWorldData info;
			info.objectClass = "oCWorld";

			while(!parser.readChunkEnd())
			{
				ZenParser::ChunkHeader header;
				parser.readChunkStart(header);

				LogInfo() << "oCWorld reading chunk: " << header.name << ", " << header.classname;

				if(header.name == "MeshAndBsp")
				{
					parser.readWorldMesh();
					parser.readChunkEnd();
				}
				else
				{
					parser.skipChunk();
				}
			}

			return info;
		}

	private:
	};

}