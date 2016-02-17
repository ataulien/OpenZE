#pragma once
#include <inttypes.h>
#include "utils/mathlib.h"

namespace ZenConvert
{
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
		Math::float2	uv;
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
}