#pragma once

namespace Utils
{
    class SystemLinux
    {
    public:
        static void mkdir(const char *path)
        {
            (void)path;
            static_assert(false, "This feature is not implemented for your OS");
        }
    };
}

