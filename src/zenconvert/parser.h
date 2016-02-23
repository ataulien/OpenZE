#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace ZenConvert
{
    class Chunk;
    class zCMesh;

    class Parser
    {
        enum EFileType
        {
            FT_UNKNOWN,
            FT_ASCII,
            FT_BINARY
        };

    public:

		struct Header
		{
			int version;
			EFileType fileType;
			bool saveGame;
			std::string date;
			std::string user;
			int objectCount;
		} m_Header;

		Parser(const std::string &fileName, Chunk *pVob = nullptr, zCMesh* pWorldMesh = nullptr);
		Parser(const std::vector<uint8_t>& fileData, Chunk *pVob = nullptr, zCMesh* pWorldMesh = nullptr);
        ~Parser();
        void parse();

        bool skipString(const std::string &pattern = std::string());
        void skipSpaces();
        void skipLine(bool silent = true);
        void skipBinaryChunk();

        void readHeader(Header* targetHeader = nullptr);
        void readWorldMesh();
        void readChunk(Chunk *pParent);
        void readBinaryChunk(Chunk *pParent);

		size_t getFileSize(){ return m_Data.size(); }

		/**
		* @brief Reads a string until \r, \n or a space is found
        */
        std::string readString(bool skipSpaces = true);

		/**
		* @brief Reads a line to \r or \n
		*/
        std::string readLine(bool skipSpaces = true);
        int32_t readInt();
        float readFloat();
        bool readBool();
		
		/**
		 * @brief Reads the given number of type T as binary data. Watch for alignment!
		 */
		template<typename T>
		void readMultipleStructures(const size_t num, std::vector<T>& out)
		{
			for(size_t i = 0; i < num; i++)
			{			
				out.emplace_back(*reinterpret_cast<T*>(&m_Data[m_Seek]));
				m_Seek += sizeof(T);
			}
		}

		/**
		* @brief Reads one structure of type T. Watch for alignment!
		*/
		template<typename T>
		void readStructure(T& s) 
		{
			s = *reinterpret_cast<T*>(&m_Data[m_Seek]);
			m_Seek += sizeof(T);
		}

		uint32_t readBinaryDword();
		uint16_t readBinaryWord();
		uint8_t readBinaryByte();

        bool isNumber();
        bool isNumber(const std::string &expr);

        void checkArraySize();

		/**
		 * @brief Returns the current position in our loaded file
		 */
		size_t getSeek() const { return m_Seek; }

		/**
		 * @brief Sets the current position in the loaded file 
		 */
		void setSeek(size_t seek) { m_Seek = seek; }

		/**
		 * @brief Returns the parsed world-mesh
		 */
		const zCMesh* getWorldMesh(){ return m_pWorldMesh; }
    private:
        Parser(Chunk *pVob, const std::vector<uint8_t> &data);
        std::vector<uint8_t> m_Data;
        std::unordered_map<uint32_t, Chunk*> m_Vobs;
        size_t m_Seek;
        Chunk *m_pVob;
        zCMesh* m_pWorldMesh;

        static const std::string s_FileFormat;
    };
}
