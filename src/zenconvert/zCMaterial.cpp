#include "zCMaterial.h"

void ZenConvert::zCMaterial::readObjectData(Parser & parser)
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

	m_MaterialInfo.matName = parser.readLine(false);
	m_MaterialInfo.matGroup; parser.readStructure(m_MaterialInfo.matGroup);
	m_MaterialInfo.color = parser.readBinaryDword();
	m_MaterialInfo.smoothAngle; parser.readStructure(m_MaterialInfo.smoothAngle);
	m_MaterialInfo.texture = parser.readLine(false);
	m_MaterialInfo.texScale = parser.readLine(false);

	m_MaterialInfo.texAniFPS; parser.readStructure(m_MaterialInfo.texAniFPS);
	m_MaterialInfo.texAniMapMode; parser.readStructure(m_MaterialInfo.matGroup);
	m_MaterialInfo.texAniMapDir = parser.readLine(false);
	m_MaterialInfo.noCollDet = parser.readBinaryByte();
	m_MaterialInfo.noLighmap = parser.readBinaryByte();
	m_MaterialInfo.loadDontCollapse = parser.readBinaryByte();
	m_MaterialInfo.detailObject = parser.readLine(false);
	m_MaterialInfo.detailTextureScale; parser.readStructure(m_MaterialInfo.detailTextureScale);
	m_MaterialInfo.forceOccluder = parser.readBinaryByte();
	m_MaterialInfo.environmentMapping = parser.readBinaryByte();
	m_MaterialInfo.environmentalMappingStrength; parser.readStructure(m_MaterialInfo.environmentalMappingStrength);
	m_MaterialInfo.waveMode; parser.readStructure(m_MaterialInfo.waveMode);
	m_MaterialInfo.waveSpeed; parser.readStructure(m_MaterialInfo.waveSpeed);
	m_MaterialInfo.waveMaxAmplitude; parser.readStructure(m_MaterialInfo.waveMaxAmplitude);
	m_MaterialInfo.waveGridSize; parser.readStructure(m_MaterialInfo.waveGridSize);
	m_MaterialInfo.ignoreSun = parser.readBinaryByte();
	m_MaterialInfo.alphaFunc; parser.readStructure(m_MaterialInfo.alphaFunc);

	// Original code seems to read a word and then THAT number of bytes directly
	// into a static buffer. Exploits here we come!

	float defaultMapping_x; parser.readStructure(defaultMapping_x);
	float defaultMapping_y; parser.readStructure(defaultMapping_y);
}
