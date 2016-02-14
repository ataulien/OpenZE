#include <iostream>

#include "parser.h"
#include "vob.h"

void dumpVob(ZenConvert::Vob *pVob, uint32_t indent = 0)
{
    for(uint32_t i = 0; i < indent; ++i)
        std::cout << "\t";
    std::cout << pVob->name() << "(" << pVob->className() << ")";
    if(pVob->reference())
        std::cout << "->" << pVob->reference()->name() << "(" << pVob->reference()->className() << ")";
    std::cout << std::endl;

    for(int i = 0; i < pVob->childCount(); ++i)
        dumpVob(pVob->child(i), indent + 1);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "You need exactly one argument. Please try again" << std::endl;
        return -1;
    }

    ZenConvert::Vob *pParentVob = new ZenConvert::Vob("parent", "", 0);
    try
    {
        ZenConvert::Parser parser(argv[1], pParentVob);
        parser.parse();
        dumpVob(pParentVob);
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    delete pParentVob;

    return 0;
}
