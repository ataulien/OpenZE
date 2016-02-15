#include <fstream>
#include <iostream>
#include <algorithm>

#include "parser.h"
#include "vob.h"
#include "utils/split.h"

const std::string ZenConvert::Parser::s_FileFormat = "ZenGin Archive";

ZenConvert::Parser::Parser(const std::string &fileName, Chunk *pVob) :
    m_Seek(0),
    m_pVob(pVob)
{
    if(!pVob)
        throw std::runtime_error("Parent vob not set");

    std::ifstream file(fileName, std::ios::in | std::ios::ate);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    if(!file.good())
        throw std::runtime_error("File does not exist");

    m_Data.resize(size);
    file.read(reinterpret_cast<char *>(m_Data.data()), size);
}

ZenConvert::Parser::~Parser()
{
}

void ZenConvert::Parser::parse()
{
    readHeader();

    readChunk(m_pVob);
}

bool ZenConvert::Parser::skipString(const std::string &pattern)
{
    skipSpaces();
    bool retVal = true;
    if(pattern.empty())
    {
        while(!(m_Data[m_Seek] == '\n' || m_Data[m_Seek] == ' '))
            ++m_Seek;
        ++m_Seek;
    }
    else
    {
        size_t lineSeek = 0;
        while(lineSeek < pattern.size())
        {
            if(m_Data[m_Seek] != pattern[lineSeek])
            {
                retVal = false;
                break;
            }

            ++m_Seek;
            ++lineSeek;
        }
    }

    return retVal;
}

void ZenConvert::Parser::skipSpaces()
{
    bool search = true;
    while(search)
    {
        checkArraySize();
        switch(m_Data[m_Seek])
        {
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            ++m_Seek;
            break;
        default:
            search = false;
            break;
        }
    }
}

void ZenConvert::Parser::skipLine(bool silent)
{
    if(!silent)
        std::cout << "skip line: ";
    while(m_Data[m_Seek] != '\n')
    {
        checkArraySize();

        if(!silent)
            std::cout << m_Data[m_Seek];
        ++m_Seek;
    }
    if(!silent)
        std::cout << std::endl;
    ++m_Seek;
}

void ZenConvert::Parser::skipBinaryChunk()
{
    uint32_t version = readBinaryDword();
    (void)version;//Don't care...
    m_Seek += readBinaryDword();
}

void ZenConvert::Parser::readHeader()
{
    if(!skipString(s_FileFormat))
        throw std::runtime_error("Not a valid format");

    if(!skipString("ver"))
        throw std::runtime_error("Not a valid header");

    m_Header.version = readInt();

    if(!skipString("zCArchiverGeneric"))
        throw std::runtime_error("Not a valid header");

    std::string fileType = readString();
    if(fileType == "ASCII")
        m_Header.fileType = FT_ASCII;
    else if(fileType == "BINARY")
        m_Header.fileType = FT_BINARY;
    else
        throw std::runtime_error("Unsupported file format");

    if(!skipString("saveGame"))
        throw std::runtime_error("Unsupported file format");

    m_Header.saveGame = readBool();

    if(skipString("date"))
    {
        m_Header.date = readString() + " ";
        m_Header.date += readString();
    }

    if(skipString("user"))
        m_Header.user = readString();

    if(!skipString("END"))
        throw std::runtime_error("No END in header(1)");

    if(!skipString("objects"))
        throw std::runtime_error("Object count missing");

    m_Header.objectCount = readInt();

    if(!skipString("END"))
        throw std::runtime_error("No END in header(2)");
}

void ZenConvert::Parser::readWorldMesh()
{
    skipBinaryChunk();
}

void ZenConvert::Parser::readChunk(Chunk *pParent)
{
    skipSpaces();
    if(m_Data[m_Seek] != '[')
        throw std::runtime_error("Invalid format");

    ++m_Seek;

    size_t tmpSeek = m_Seek;
    while(m_Data[tmpSeek] != ']')
    {
        if(m_Data[tmpSeek] == '\r' && m_Data[tmpSeek] == '\n')
            throw std::runtime_error("Invalid vob descriptor");

        ++tmpSeek;
    }

    std::string vobDescriptor(reinterpret_cast<char *>(&m_Data[m_Seek]), tmpSeek - m_Seek);
    std::vector<std::string> vec = Utils::split(vobDescriptor, ' ');
    m_Seek = tmpSeek + 1;

    std::string name;
    std::string className;
    int classVersion = 0;
    int objectID = 0;
    bool createObject = false;
    enum State
    {
        S_OBJECT_NAME,
        S_REFERENCE,
        S_CLASS_NAME,
        S_CLASS_VERSION,
        S_OBJECT_ID,
        S_FINISHED
    } state = S_OBJECT_NAME;

    for(auto &arg : vec)
    {
        switch(state)
        {
        case S_OBJECT_NAME:
            if(arg != "%")
            {
                name = arg;
                state = S_REFERENCE;
                break;
            }
        case S_REFERENCE:
            if(arg == "%")
            {
                createObject = true;
                state = S_CLASS_NAME;
                break;
            }
            else if(arg == "\xA7")
            {
                createObject = false;
                state = S_CLASS_NAME;
                break;
            }
            else
                createObject = true;
        case S_CLASS_NAME:
            if(!isNumber(arg))
            {
                className = arg;
                state = S_CLASS_VERSION;
                break;
            }
        case S_CLASS_VERSION:
            classVersion = std::atoi(arg.c_str());
            state = S_OBJECT_ID;
            break;
        case S_OBJECT_ID:
            objectID = std::atoi(arg.c_str());
            state = S_FINISHED;
            break;
        default:
            throw std::runtime_error("Strange parser state");
        }
    }

    if(state != S_FINISHED)
        throw std::runtime_error("Parser did not finish");

    Chunk *pRef = nullptr;
    if(!createObject)
    {
        if(m_Vobs.find(objectID) == m_Vobs.end())
            throw std::runtime_error("Reference to unknown object: " + std::to_string(objectID));
        pRef = m_Vobs[objectID];
    }
    Chunk *pVob = pParent->addVob(name, className, classVersion, objectID, pRef);
    m_Vobs.emplace(objectID, pVob);

    if(!pVob)
        throw std::runtime_error("references are not implemented yet " + std::to_string(m_Seek));

    if(name == "MeshAndBsp")
    {
        ++m_Seek;
        readWorldMesh();
    }

    skipSpaces();
    while(true)
    {
        skipSpaces();
        if((m_Data[m_Seek] == '[') && (m_Data[m_Seek + 1] == ']'))
        {
            skipLine();
            break;
        }

        if(m_Data[m_Seek] == '[')
        {
            readChunk(pVob);
            continue;
        }
        else
        {
            auto vec = Utils::split(readLine(), '=');
            if(vec.size() != 2)
                throw std::runtime_error("Missing argument " + std::to_string(m_Seek));
            pVob->addAttribute(vec[0], vec[1]);
        }
    }
}

void ZenConvert::Parser::readBinaryChunk(ZenConvert::Chunk *pParent)
{
    uint32_t version = readBinaryDword();
    (void)version;//nvm
    uint32_t size = readBinaryDword();

}

std::string ZenConvert::Parser::readString(bool skip)
{
    if(skip)
        skipSpaces();

    std::string str;
    while(m_Data[m_Seek] != '\r' && m_Data[m_Seek] != '\n' && m_Data[m_Seek] != ' ')
    {
        str += m_Data[m_Seek];
        ++m_Seek;
    }
    return str;
}

std::string ZenConvert::Parser::readLine()
{
    std::string retVal;
    while(m_Data[m_Seek] != '\r' && m_Data[m_Seek] != '\n')
    {
        checkArraySize();
        retVal += m_Data[m_Seek++];
    }
    skipSpaces();
    return retVal;
}

int32_t ZenConvert::Parser::readInt()
{
    skipSpaces();
    std::string number;
    while(m_Data[m_Seek] >= '0' && m_Data[m_Seek] <= '9')
    {
        number += m_Data[m_Seek];
        ++m_Seek;
    }
    return std::stoi(number);
}

float ZenConvert::Parser::readFloat()
{
    //Todo;
    throw std::runtime_error("Not implemented: " + (__PRETTY_FUNCTION__ + std::to_string(__LINE__)));
    return 0.0f;
}

bool ZenConvert::Parser::readBool()
{
    skipSpaces();
    bool retVal;
    if(m_Data[m_Seek] != '0' && m_Data[m_Seek] != '1')
        throw std::runtime_error("Value is not a bool");
    else
        retVal = m_Data[m_Seek] == '0' ? false : true;

    ++m_Seek;
    return retVal;
}

uint32_t ZenConvert::Parser::readBinaryDword()
{
    uint32_t retVal = *reinterpret_cast<uint32_t *>(&m_Data[m_Seek]);
    m_Seek += sizeof(uint32_t);
    return retVal;
}

bool ZenConvert::Parser::isNumber()
{
    skipSpaces();
    bool retVal = true;
    size_t tmpSeek = m_Seek;
    while(m_Data[tmpSeek] != '\n' || m_Data[tmpSeek] != ' ')
    {
        if(!(m_Data[tmpSeek] <= '9' && m_Data[tmpSeek] >= '0'))
            retVal = false;
        ++tmpSeek;
    }
    return retVal;
}

bool ZenConvert::Parser::isNumber(const std::string &expr)
{
    return !expr.empty() && std::find_if(expr.begin(), expr.end(), [](char c){return !std::isdigit(c); }) == expr.end();
}

void ZenConvert::Parser::checkArraySize()
{
    if(m_Seek >= m_Data.size())
        throw std::logic_error("Out of range");
}

ZenConvert::Parser::Parser(ZenConvert::Chunk *pVob, const std::vector<uint8_t> &data) :
    m_Data(data),
    m_Seek(0),
    m_pVob(pVob)
{
}
