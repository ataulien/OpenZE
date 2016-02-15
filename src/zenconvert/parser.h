#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace ZenConvert
{
    class Chunk;

    class Parser
    {
        enum EFileType
        {
            FT_UNKNOWN,
            FT_ASCII,
            FT_BINARY
        };

        struct Header
        {
            int version;
            EFileType fileType;
            bool saveGame;
            std::string date;
            std::string user;
            int objectCount;
        } m_Header;

    public:
        Parser(const std::string &fileName, Chunk *pVob = nullptr);
        ~Parser();
        void parse();

        bool skipString(const std::string &pattern = std::string());
        void skipSpaces();
        void skipLine(bool silent = true);
        void skipBinaryChunk();

        void readHeader();
        void readWorldMesh();
        void readChunk(Chunk *pParent);
        void readBinaryChunk(Chunk *pParent);
        std::string readString(bool skipSpaces = true);
        std::string readLine();
        int32_t readInt();
        float readFloat();
        bool readBool();

        uint32_t readBinaryDword();

        bool isNumber();
        bool isNumber(const std::string &expr);

        void checkArraySize();

    private:
        Parser(Chunk *pVob, const std::vector<uint8_t> &data);
        std::vector<uint8_t> m_Data;
        std::unordered_map<uint32_t, Chunk*> m_Vobs;
        size_t m_Seek;
        Chunk *m_pVob;

        static const std::string s_FileFormat;
    };
}
