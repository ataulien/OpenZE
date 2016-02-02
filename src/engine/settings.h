#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace Engine
{
    /**
     * @brief The Settings class
     */
    class Settings
    {
    public:
        /**
         * @brief Settings
         * @param argc
         * @param argv
         */
        Settings(int argc, char *argv[]);

    private:
        /**
         * @brief arguments passed to the game on startup
         */
        std::unordered_map<std::string, std::string> m_Arguments;

        /**
         * @brief all available arguments
         */
        static std::unordered_set<std::string> s_AvailableArguments;
    };
}
