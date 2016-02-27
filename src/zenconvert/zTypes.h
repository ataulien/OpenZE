#pragma once
#include <inttypes.h>
#include "utils/mathlib.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace ZenConvert
{
	/**
	 * @brief Base for an object parsed from a zen-file.
	 *		  Contains a map of all properties and their values as text
	 */
	struct ParsedZenObject
	{
		std::unordered_map<std::string, std::string> properties;
		std::string objectClass;
	};


	/**
	* @brief All kinds of information found in a zCMaterial
	*/
	struct zCMaterialData : public ParsedZenObject
	{
		std::string		matName;
		uint8_t			matGroup;
		uint32_t		color;
		float			smoothAngle;
		std::string		texture;
		std::string		texScale;
		float			texAniFPS;
		uint8_t			texAniMapMode;
		std::string		texAniMapDir;
		uint8_t			noCollDet;
		uint8_t			noLighmap;
		uint8_t			loadDontCollapse;
		std::string		detailObject;
		float			detailTextureScale;
		uint8_t			forceOccluder;
		uint8_t			environmentMapping;
		float			environmentalMappingStrength;
		uint8_t			waveMode;
		uint8_t			waveSpeed;
		float			waveMaxAmplitude;
		float			waveGridSize;
		uint8_t			ignoreSun;
		uint8_t			alphaFunc;
		Math::float2	defaultMapping;
	};

	struct zCVisualData : public ParsedZenObject
	{

	};

	struct zCAIBaseData : public ParsedZenObject
	{

	};

	struct zCEventManagerData : public ParsedZenObject
	{

	};

	/**
	 * @brief Data of zCVob
	 */
	struct zCVobData : public ParsedZenObject
	{
		uint32_t pack;
		std::string presetName;
		Math::float3 bboxMin;
		Math::float3 bboxMax;
		Math::float3 rotationMatrixRows[3];
		Math::float3 position;
		std::string vobName;
		std::string visual;
		bool showVisual;
		uint8_t visualCamAlign;
		uint8_t visualAniMode;
		float visualAniModeStrength;
		float vobFarClipScale;
		bool cdStatic;
		bool cdDyn;
		bool staticVob;
		uint8_t dynamicShadow;
		int32_t zBias;
		bool isAmbient;

		// References
		size_t visualReference;
		size_t aiReference;
		size_t eventMgrReference;

		bool physicsEnabled;

		std::vector<zCVobData> childVobs;
	};

	/**
	* @brief All kinds of information found in a oCWorld
	*/
	struct oCWorldData : public ParsedZenObject
	{
		zCVobData rootVob;
	};

#pragma pack(push, 1)
	// Information about the whole file we are reading here
	struct BinaryFileInfo
	{
		uint32_t version;
		uint32_t size;
	};

	// Information about a single chunk 
	struct BinaryChunkInfo
	{
		uint16_t id;
		uint32_t length;
	};

	struct PolyFlags 
	{
		uint8_t			portalPoly			: 2;		
		uint8_t			occluder			: 1;		
		uint8_t			sectorPoly			: 1;		
		uint8_t			mustRelight			: 1;		
		uint8_t			portalIndoorOutdoor	: 1;		
		uint8_t			ghostOccluder		: 1;		
		uint8_t			noDynLightNear		: 1;		
		uint16_t		sectorIndex			: 16;
	};

	struct zTMSH_FeatureChunk 
	{
		float			uv[2];
		uint32_t		lightStat;
		Math::float3	vertNormal;
	};

#pragma pack(pop)

#pragma pack(push, 4)
	struct zDate
	{
		uint32_t year;
		uint16_t month;
		uint16_t day;	
		uint16_t hour;
		uint16_t minute;
		uint16_t second;
	};

	struct zTPlane 
	{
		float distance;
		Math::float3 normal;
	};
#pragma pack(pop)

	struct zWedge 
	{
		Math::float3 m_Normal;
		Math::float2 m_Texcoord;
		uint16_t m_VertexIndex;
	};

	struct zTriangle
	{
		uint16_t m_Wedges[3];
	};													

	struct zTriangleEdges 
	{
		uint16_t m_Edges[3];
	};													

	struct zEdge 
	{
		uint16_t m_Wedges[2];
	};													
}
