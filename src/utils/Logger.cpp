#include "logger.h"

#ifdef USE_LOG

namespace Utils
{
	// Definition of the static log callback
	std::function<void(const std::string&)> Log::s_LogCallback;
	std::string Log::s_LogFile;
}

#endif