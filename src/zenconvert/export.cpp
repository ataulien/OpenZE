#include <iostream>

#include "export.h"
#include "vob.h"
#include "utils/system.h"
#include "utils/logger.h"

ZenConvert::Export::Export() :
    m_Indent(0)
{
}

void ZenConvert::Export::exportVobTree(Chunk *pVob)
{
//    for(uint32_t i = 0; i < m_Indent; ++i)
//        std::cout << "\t";
//    std::cout << pVob->name() << "(" << pVob->className() << ")";
//    if(pVob->reference())
//        std::cout << "->" << pVob->reference()->name() << "(" << pVob->reference()->className() << ")";
//    std::cout << std::endl;

//    ++m_Indent;
//    for(int i = 0; i < pVob->childCount(); ++i)
//        exportVobTree(pVob->child(i));

//    --m_Indent;

    pVob = pVob->child(0);
    std::cout << pVob->name() << "(" << pVob->className() << ")" << std::endl;

    SYS::mkdir("output");
    for(int i = 0; i < pVob->childCount(); ++i)
    {
        Chunk *pChild = pVob->child(i);
        if(pChild->name() == "MeshAndBsp")
        {
            std::ofstream of;
            exportMaterials(pChild, of);
            exportWorld(pChild, of);
        }
        else if(pChild->name() == "VobTree")
        {
            std::ofstream of;
            exportVobs(pChild, of);
        }
        else if(pChild->name() == "WayNet")
        {
            std::ofstream of("output/waynet.json");
            exportWayNet(pChild, of);
        }
        else if(pChild->name() == "EndMarker")
            LogInfo() << "EOF";
        else
            throw std::runtime_error("this top level vob is not supported");
    }
}

void ZenConvert::Export::exportWayNet(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    (void)stream;
    std::cout << pVob->name() << std::endl;
}

void ZenConvert::Export::exportMaterials(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    (void)stream;
    std::cout << pVob->name() << std::endl;
}

void ZenConvert::Export::exportWorld(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    (void)stream;
    std::cout << pVob->name() << std::endl;
}

void ZenConvert::Export::exportVobs(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    (void)stream;
    std::cout << pVob->name() << std::endl;
}
