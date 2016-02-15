#pragma once

#include <inttypes.h>
#include <fstream>

namespace ZenConvert
{
    class Chunk;

    class Export
    {
    public:
        Export();
        virtual void exportVobTree(Chunk *pVob);
        virtual void exportWayNet(Chunk *pVob, std::ostream &stream);
        virtual void exportMaterials(Chunk *pVob, std::ostream &stream);
        virtual void exportWorld(Chunk *pVob, std::ostream &stream);
        virtual void exportVobs(Chunk *pVob, std::ostream &stream);

    private:
        uint32_t m_Indent;
    };
}
