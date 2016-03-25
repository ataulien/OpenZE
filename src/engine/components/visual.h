#pragma once

#include <REngine.h>
#include <RResourceCache.h>
#include <RPipelineState.h>

#include "components.h"
#include "utils/mathlib.h"

namespace RAPI
{
    class RBuffer;
    struct RPipelineState;
}

namespace Engine
{
    namespace Components
    {
        struct Visual
        {
            enum { MASK = C_VISUAL };

			// Optional buffer attached to an entity, in case instancing is not used
            RAPI::RBuffer* pObjectBuffer;
			RAPI::RPipelineState* pPipelineState;
			RAPI::RBuffer* pInstanceBuffer;

			// Color-modification for this instance
			// TODO: Put this into a per-instance-buffer
			Math::float4 colorMod;

			size_t visualId;
			size_t visualSubId;

            void cleanUp()
            {
                RAPI::REngine::ResourceCache->DeleteResource(pObjectBuffer);
                RAPI::REngine::ResourceCache->DeleteResource(pPipelineState);
            }
        };
    }
}

