#pragma once

#include "utils/mathlib.h"
#include "RInputLayout.h"

namespace Renderer
{
    /** We pack most of Gothics FVF-formats into this vertex-struct */
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

}
