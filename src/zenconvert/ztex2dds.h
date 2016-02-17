#pragma once
#include <vector>
#include <inttypes.h>

namespace ZenConvert
{
	/**
	* @brief Modified ZTEX to DDS conversion
	*/
	int convertZTEX2DDS( const std::vector<uint8_t>& ztexData, std::vector<uint8_t>& ddsData, bool optionForceARGB = false);
}