#include "parserImplASCII.h"
#include "utils/logger.h"

using namespace ZenConvert;

ParserImplASCII::ParserImplASCII(ZenParser * parser) : ParserImpl(parser)
{
}

/**
 * @brief Read the start of a chunk. [...]
 */
void ParserImplASCII::readChunkStart(ZenParser::ChunkHeader& header)
{
	// Check chunk-header
	m_pParser->skipSpaces();
	if(m_pParser->m_Data[m_pParser->m_Seek] != '[')
		throw std::runtime_error("Invalid format");

	m_pParser->m_Seek++;

	size_t tmpSeek = m_pParser->m_Seek;
	while(m_pParser->m_Data[tmpSeek] != ']')
	{
		if(m_pParser->m_Data[tmpSeek] == '\r' && m_pParser->m_Data[tmpSeek] == '\n')
			throw std::runtime_error("Invalid vob descriptor");

		++tmpSeek;
	}

	// Save chunks starting-position (right after chunk-header)
	header.startPosition = m_pParser->m_Seek;

	// Parse chunk-header
	std::string vobDescriptor(reinterpret_cast<char *>(&m_pParser->m_Data[m_pParser->m_Seek]), tmpSeek - m_pParser->m_Seek);
	std::vector<std::string> vec = Utils::split(vobDescriptor, ' ');
	m_pParser->m_Seek = tmpSeek + 1;

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
			if(!m_pParser->isNumber(arg))
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

	header.classname = className;
	header.createObject = createObject;
	header.name = name;
	header.objectID = objectID;
	header.size = 0; // Doesn't matter in ASCII
	header.version = classVersion;

	// Skip the last newline
	m_pParser->skipSpaces();
}

/**
 * @brief Read the end of a chunk. []
 */
void ParserImplASCII::readChunkEnd()
{
	std::string l = readString();

	if(l != "[]")
		throw std::runtime_error("Failed to find Chunk-End");
}

/**
 * @brief Read the implementation specific header and stores it in the parsers main-header struct
 */
void ParserImplASCII::readImplHeader()
{
	if(!m_pParser->skipString("objects"))
		throw std::runtime_error("Object count missing");

	m_pParser->m_Header.objectCount = m_pParser->readIntASCII();

	if(!m_pParser->skipString("END"))
		throw std::runtime_error("No END in header(2)");

	m_pParser->skipSpaces();
}

/**
 * @brief Reads a string
 */
std::string ParserImplASCII::readString()
{
	return m_pParser->readLine();
}

/**
 * @brief Reads data of the expected type. Throws if the read type is not the same as specified and not 0
 */
void ParserImplASCII::readEntry(const std::string& expectedName, void* target, size_t targetSize, EZenValueType expectedType)
{
	m_pParser->skipSpaces();
	std::string line = m_pParser->readLine();

	// Special cases for chunk starts/ends
	if(line == "[]" || (line.front() == '[' && line.back() == ']'))
	{
		*reinterpret_cast<std::string*>(target) = line;
		return;
	}

	// Split at =, then at :
	// parts should then have 3 elements, name, type and value
	auto parts = Utils::split(line, "=:");

	if(parts.size() < 2)
		throw std::runtime_error("Failed to parse property: " + expectedName);

	const std::string& valueName = parts[0];
	const std::string& type = parts[1];
	const std::string& value = parts.size() > 2 ? parts[2] : "";

	// Split value as well, if possible
	auto vparts = Utils::split(line, ' ');

	if(!expectedName.empty() && valueName != expectedName)
		throw std::runtime_error("Value name does not match expected name. Value:" + valueName + " Expected: " + expectedName);

	switch(expectedType)
	{
		case ZVT_0: break;
		case ZVT_STRING: *reinterpret_cast<std::string*>(target) = value; break;
		case ZVT_INT: *reinterpret_cast<int32_t*>(target) = std::stoi(value); break;
		case ZVT_FLOAT: *reinterpret_cast<float*>(target) = std::stof(value); break;
		case ZVT_BYTE: *reinterpret_cast<uint8_t*>(target) = static_cast<uint8_t>(std::stoi(value)); break;
		case ZVT_WORD: *reinterpret_cast<int16_t*>(target) = static_cast<int16_t>(std::stoi(value)); break;
		case ZVT_BOOL: *reinterpret_cast<bool*>(target) = std::stoi(value) != 0; break;
		case ZVT_VEC3: *reinterpret_cast<Math::float3*>(target) = Math::float3(std::stof(vparts[0]), std::stof(vparts[1]), std::stof(vparts[2])); break;
		
		case ZVT_COLOR: 
			reinterpret_cast<uint8_t*>(target)[0] = std::stoi(vparts[0]); // FIXME: These are may ordered wrong
			reinterpret_cast<uint8_t*>(target)[1] = std::stoi(vparts[1]);
			reinterpret_cast<uint8_t*>(target)[2] = std::stoi(vparts[2]);
			reinterpret_cast<uint8_t*>(target)[3] = std::stoi(vparts[3]);
			break;

		case ZVT_RAW_FLOAT:
		case ZVT_RAW: 
			{
				uint8_t* data = reinterpret_cast<uint8_t*>(target);
				char c[3];
				c[2] = 0;

				for(size_t i = 0; i < targetSize; i++)
				{
					if(value.size() < i * 2 + 1)
						throw std::runtime_error("Invalid raw dataset");

					c[0] = value[i * 2];
					c[1] = value[i * 2 + 1];
					data[i] = static_cast<uint8_t>(std::stoi(std::string(c), 0, 16));
				}
			}
			break;
		case ZVT_10: break;
		case ZVT_11: break;
		case ZVT_12: break;
		case ZVT_13: break;
		case ZVT_14: break;
		case ZVT_15: break;
		case ZVT_ENUM: *reinterpret_cast<uint8_t*>(target) = std::stoi(value); break;
	}
}

/**
* @brief Reads the type of a single entry
*/
void ParserImplASCII::readEntryType(EZenValueType& outtype, size_t& size)
{
	m_pParser->skipSpaces();
	std::string line = m_pParser->readLine();

	// Special cases for chunk starts/ends
	if(line == "[]" || (line.front() == '[' && line.back() == ']'))
	{
		outtype = ZVT_STRING;
		size = line.length();
		return;
	}

	// Split at =, then at :
	// parts should then have 3 elements, name, type and value
	auto parts = Utils::split(line, "=:");

	if(parts.size() < 2) // Need at least name and type
		throw std::runtime_error("Failed to read property type");

	const std::string& type = parts[1];

	if(type == "string") { outtype = ZVT_STRING; size = line.length(); }
	else if(type == "int") { outtype = ZVT_INT; size = sizeof(int32_t); }
	else if(type == "float") { outtype = ZVT_FLOAT; size = sizeof(float); }
	else if(type == "byte") { outtype = ZVT_BYTE; size = sizeof(uint8_t); }
	else if(type == "word") { outtype = ZVT_WORD; size = sizeof(int16_t); }
	else if(type == "bool") { outtype = ZVT_BOOL; size = sizeof(uint8_t); }
	else if(type == "vec3") { outtype = ZVT_VEC3; size = sizeof(Math::float3); }
	else if(type == "color") { outtype = ZVT_COLOR; size = sizeof(uint32_t); }
	else if(type == "rawFloat") { outtype = ZVT_RAW_FLOAT; size = line.length(); }
	else if(type == "raw") { outtype = ZVT_RAW; size = line.length(); }
	else if(type == "enum") { outtype = ZVT_ENUM; size = sizeof(uint8_t); }
	else throw std::runtime_error("Unknown type");
}