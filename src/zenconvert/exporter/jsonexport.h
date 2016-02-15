#pragma once

#include "../export.h"

namespace ZenConvert
{
    class JsonExport : public Export
    {
    public:
        virtual void exportWayNet(Chunk *pVob, std::ostream &stream) override;
        virtual void exportVobs(Chunk *pVob, std::ostream &stream) override;
    };
}
