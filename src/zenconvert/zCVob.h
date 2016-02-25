#pragma once
#include "zTypes.h"
#include "parser.h"

namespace ZenConvert
{
	class zCVob
	{
	public:
		/**
		* Reads this object from an internal zen
		*/
		static MaterialInfo readObjectData(Parser& parser)
		{
			std::string matName = parser.readLine(false);
			//LogInfo() << "Reading material '" << matName << "'";

			uint32_t chunksize = parser.readBinaryDword();
			uint16_t version = parser.readBinaryWord();
			uint32_t objectIndex = parser.readBinaryDword();

			parser.skipSpaces();

			// Skip chunk-header
			std::string name = parser.readLine();
			std::string classname = parser.readLine();

			// Read everything the material has to offer

			MaterialInfo materialInfo;
			materialInfo.matName = parser.readLine(false);
			materialInfo.matGroup = parser.readBinaryByte();
			materialInfo.color = parser.readBinaryDword();
			materialInfo.smoothAngle = parser.readBinaryFloat();
			materialInfo.texture = parser.readLine(false);
			materialInfo.texScale = parser.readLine(false);
			materialInfo.texAniFPS = parser.readBinaryFloat();
			materialInfo.texAniMapMode = parser.readBinaryByte();
			materialInfo.texAniMapDir = parser.readLine(false);
			materialInfo.noCollDet = parser.readBinaryByte();
			materialInfo.noLighmap = parser.readBinaryByte();
			materialInfo.loadDontCollapse = parser.readBinaryByte();
			materialInfo.detailObject = parser.readLine(false);
			materialInfo.detailTextureScale = parser.readBinaryFloat();
			materialInfo.forceOccluder = parser.readBinaryByte();
			materialInfo.environmentMapping = parser.readBinaryByte();
			materialInfo.environmentalMappingStrength = parser.readBinaryFloat();
			materialInfo.waveMode = parser.readBinaryByte();
			materialInfo.waveSpeed = parser.readBinaryByte();
			materialInfo.waveMaxAmplitude = parser.readBinaryFloat();
			materialInfo.waveGridSize = parser.readBinaryFloat();
			materialInfo.ignoreSun = parser.readBinaryByte();
			materialInfo.alphaFunc = parser.readBinaryByte();

			float defaultMapping_x = parser.readBinaryFloat();
			float defaultMapping_y = parser.readBinaryFloat();

			return materialInfo;
		}

	private:
	};
}