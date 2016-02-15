#pragma once

#include "../export.h"

namespace ZenConvert
{
    class JsonExport : public Export
    {
    public:
        JsonExport(const std::string &dirName);
        virtual void exportWayNet(Chunk *pVob, std::ostream &stream) override;
        virtual void exportVobs(Chunk *pVob, std::ostream &stream) override;
    };
}
