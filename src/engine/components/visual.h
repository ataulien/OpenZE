#pragma once

#include <REngine.h>
#include <RResourceCache.h>

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

            RAPI::RBuffer *pObjectBuffer;
            RAPI::RPipelineState *pPipelineState;
			Math::Matrix tmpWorld; // TODO: Put this into some kind of Transform-Component!
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

