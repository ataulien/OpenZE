#include "jsonexport.h"
#include "../vob.h"
#include "utils/split.h"

#include <iostream>

ZenConvert::JsonExport::JsonExport(const std::string &dirName) :
    Export(dirName)
{
}

void ZenConvert::JsonExport::exportWayNet(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    stream << "{\n";
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

std::string getString(const std::string &str)
{
    std::string newString = str;
    size_t s = newString.find(":") + 1;
    newString.erase(0,  s);
    return newString;
}

std::string getMat4(const std::string &mat)
{
    if(mat.length() / 8 != 9)
        throw std::runtime_error("Invalid matrix");

    std::string matrix;
    bool first = true;
    for(uint8_t i = 0; i < 9; ++i)
    {
        if(first)
            first = false;
        else
            matrix += ", ";

        float f;
        std::stringstream stream;
        stream << mat.substr(i * 8, 8);
        stream >> std::hex >> f;
        matrix += std::to_string(f);
    }

    return matrix;
}

void ZenConvert::JsonExport::exportVobs(ZenConvert::Chunk *pVob, std::ostream &stream)
{
    stream << "{\n";

    bool firstLoop = true;
    for(uint32_t i = 0, chunkCount = pVob->childCount(); i < chunkCount; ++i)
    {
        if(firstLoop)
            firstLoop = false;
        else
            stream << ",\n";

        stream << "\t\"" << std::to_string(pVob->child(i)->objectID()) << "\": {\n";
        bool first = true;
        bool edited = false;
        std::string term = "\t\t";
        for(auto &attribute : pVob->child(i)->attributes())
        {
            if(attribute.first == "visual")
            {
                std::string visual = getString(attribute.second);
                if(!visual.empty())
                {
                    edited = true;
                    stream << term << "\"visual\": \"" << visual << "\"\n";
                }
            }
            else if(attribute.first == "trafoOSToWSRot")
            {
                edited = true;
                stream << term << "\"transform\": [ " << getMat4(getString(attribute.second)) << " ]\n";
            }
            else
                std::cout << "unhandled attribute: " << attribute.first << ": " << attribute.second << std::endl;

            if(first && edited)
            {
                first = false;
                term = "," + term;
            }
        }
        stream << "\t}";
    }
    stream << "\n}\n";
}
