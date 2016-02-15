#include <iostream>

#include "parser.h"
#include "vob.h"
#include "exporter/jsonexport.h"
#include "zCMesh.h"
#include "utils/split.h"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "You need exactly one argument. Please try again" << std::endl;
        return -1;
    }

    ZenConvert::Chunk *pParentVob = new ZenConvert::Chunk("parent", "", 0);
	ZenConvert::zCMesh worldMesh;

    try
    {
        ZenConvert::Parser parser(argv[1], pParentVob, &worldMesh);
        parser.parse();

        ZenConvert::JsonExport exporter(Utils::split(argv[1], '.')[0]);
        exporter.exportVobTree(pParentVob);
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    delete pParentVob;

    return 0;
}
