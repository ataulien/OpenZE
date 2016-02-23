#pragma once

#include "utils/mathlib.h"
#include "RInputLayout.h"

namespace Renderer
{
    
	struct SimpleVertex
	{
		static const RAPI::INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[2];

		Math::float3 Position;
		Math::float3 Normal;

	};

	SELECTANY const RAPI::INPUT_ELEMENT_DESC SimpleVertex::INPUT_LAYOUT_DESC[2] =
	{
		{ "POSITION", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
	};

	struct WorldVertex
	{
		static const RAPI::INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[4];

		Math::float3 Position;
		Math::float3 Normal;
		Math::float2 TexCoord;
		uint32_t Color;

	};

	SELECTANY const RAPI::INPUT_ELEMENT_DESC WorldVertex::INPUT_LAYOUT_DESC[4] =
	{
		{ "POSITION", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, RAPI::FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, RAPI::FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
	};

}
