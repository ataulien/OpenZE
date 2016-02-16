#pragma once

#ifdef WIN32
#include "systemwindows.h"
#define SYS Utils::SystemWindows
#else
#include "systemlinux.h"
#define SYS Utils::SystemLinux
#endif
