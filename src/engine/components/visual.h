#pragma once

#ifdef ZE_GAME

#include <REngine.h>
#include <RResourceCache.h>

#include "components.h"

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

            void cleanUp()
            {
                //RAPI::REngine::ResourceCache->DeleteResource(pObjectBuffer);
                //RAPI::REngine::ResourceCache->DeleteResource(pPipelineState);
            }
        };
    }
}
#endif
