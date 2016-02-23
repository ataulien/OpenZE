#pragma once

#ifdef ZE_GAME

#include <REngine.h>
#include <RResourceCache.h>

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
            RAPI::RBuffer *pObjectBuffer;
            RAPI::RPipelineState *pPipelineState;

            void cleanUp()
            {
                RAPI::REngine::ResourceCache->DeleteResource(pObjectBuffer);
                RAPI::REngine::ResourceCache->DeleteResource(pPipelineState);
            }
        };
    }
}
#endif
