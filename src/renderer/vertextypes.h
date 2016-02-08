#pragma once

#include "utils/mathlib.h"
#include "RInputLayout.h"

namespace Renderer
{
    /** We pack most of Gothics FVF-formats into this vertex-struct */
    struct SimpleVertex
    {
        static const RAPI::INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[1];

        Math::float3 Position;

    };

    SELECTANY const RAPI::INPUT_ELEMENT_DESC SimpleVertex::INPUT_LAYOUT_DESC[1] =
    {
        { "POSITION", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
    };

}
