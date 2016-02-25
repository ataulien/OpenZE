#pragma once
#include "parserImpl.h"

namespace ZenConvert
{
	class ParserImplBinary : public ParserImpl
	{
		friend ZenParser;
	public:
		ParserImplBinary(ZenParser* parser);

		/**
		 * @brief Read the implementation specific header and stores it in the parsers main-header struct
		 */
		virtual void readImplHeader();

		/**
		 * @brief Read the start of a chunk. [...]
		 */
		virtual void readChunkStart(ZenParser::ChunkHeader& header);

		/**
		 * @brief Read the end of a chunk. []
		 */
		virtual void readChunkEnd();

		/**
		 * @brief Reads a string
		 */
		virtual std::string readString();

		/**
		 * @brief Reads data of the expected type. Throws if the read type is not the same as specified and not 0
		 */
		virtual void readEntry(const std::string& name, void* target, size_t targetSize, EZenValueType expectedType = ZVT_0);

		/**
		* @brief Reads the type of a single entry
		*/
		virtual void readEntryType(EZenValueType& type, size_t& size);
	};
}