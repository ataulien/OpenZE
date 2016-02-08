#pragma once

#ifdef ZE_GAME
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
        };
    }
}
#endif
