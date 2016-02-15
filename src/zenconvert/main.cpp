#include <iostream>

#include "parser.h"
#include "vob.h"
#include "exporter/jsonexport.h"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "You need exactly one argument. Please try again" << std::endl;
        return -1;
    }

    ZenConvert::Chunk *pParentVob = new ZenConvert::Chunk("parent", "", 0);
    try
    {
        ZenConvert::Parser parser(argv[1], pParentVob);
        parser.parse();

        ZenConvert::JsonExport exporter;
        exporter.exportVobTree(pParentVob);
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    delete pParentVob;

    return 0;
}
