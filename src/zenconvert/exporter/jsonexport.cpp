#include "jsonexport.h"
#include "../vob.h"
#include "utils/split.h"

#include <iostream>

void ZenConvert::JsonExport::exportWayNet(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    stream << "{\n";
    uint32_t chunkCount;
    pVob = pVob->child(0);
    bool first = true;
    for(uint32_t i = 0, chunkCount = pVob->childCount(); i < chunkCount; ++i)
    {
        Chunk *pChild = pVob->child(i);
        if(pChild->reference())
            continue;
        if(first)
            first = false;
        else
            stream << ",\n";
        stream << "\t";

        auto &attributes = pChild->attributes();
        if(attributes.find("wpName") == attributes.end())
            throw std::runtime_error("Waypoint without name");

        if(attributes.find("direction") == attributes.end())
            throw std::runtime_error("Waypoint without direction");

        if(attributes.find("position") == attributes.end())
            throw std::runtime_error("Waypoint without position");

        std::string name = attributes["wpName"];
        name = Utils::split(name, ':')[1];
        stream << "\"" << name << "\": {\n";

        stream << "\t\t" << "\"dir\": [ " << Utils::replaceString(Utils::split(attributes["direction"], ':')[1], " ", ", ") << " ],\n";
        stream << "\t\t" << "\"pos\": [ " << Utils::replaceString(Utils::split(attributes["position"], ':')[1], " ", ", ") << " ]\n";
        stream << "\t}";
    }
    stream << "\n}\n";
}

void ZenConvert::JsonExport::exportVobs(ZenConvert::Chunk *pVob, std::ostream &stream)
{
}
