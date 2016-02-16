#include <iostream>

#include "export.h"
#include "vob.h"
#include "utils/system.h"
#include "utils/logger.h"

ZenConvert::Export::Export(const std::string &dirName) :
    m_DirectoryName(dirName)
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

    SYS::mkdir(m_DirectoryName.c_str());

    std::ofstream vobTreeStream(m_DirectoryName + "/vobtree.json");
    std::ofstream wayNetStream(m_DirectoryName + "/waypoints.json");
    for(int i = 0; i < pVob->childCount(); ++i)
    {
        Chunk *pChild = pVob->child(i);
        if(pChild->name() == "MeshAndBsp")
        {
            exportMaterials(pChild, std::cout);
            exportWorld(pChild, std::cout);
        }
        else if(pChild->name() == "VobTree")
        {
            exportVobs(pChild, vobTreeStream);
        }
        else if(pChild->name() == "WayNet")
        {
            exportWayNet(pChild, wayNetStream);
        }
        else if(pChild->name() == "EndMarker")
            LogInfo() << "EOF";
        else
            throw std::runtime_error("this top level vob is not supported");
    }
}

void ZenConvert::Export::exportWayNet(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    stream << pVob->name() << std::endl;
}

void ZenConvert::Export::exportMaterials(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    stream << pVob->name() << std::endl;
}

void ZenConvert::Export::exportWorld(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    stream << pVob->name() << std::endl;
}

void ZenConvert::Export::exportVobs(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    stream << pVob->name() << std::endl;
}
