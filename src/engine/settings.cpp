#include <vector>
#include <iostream>

#include "settings.h"
#include "utils/split.h"

Engine::Settings::Settings(int argc, char *argv[])
{
    for(int i = 1; i < argc; ++i)
    {
        std::vector<std::string> arg = Utils::split(argv[i], '=');
        //the format is not like "<argument>=<value>" if the result vector does not have 2 elements
        if(arg.size() != 2)
        {
            std::cout << arg.size() << std::endl;
            std::cerr << "Argument invalid format: " << argv[i] << std::endl << "Should be: <argument>=<value>" << std::endl;
            exit(-1);
        }

        //check if the argument is available
        if(s_AvailableArguments.find(arg[0]) == s_AvailableArguments.end())
        {
            std::cerr << "Invalid parameter: " << argv[i] << std::endl;
            exit(-1);
        }

        //check if the argument was set before
        if(m_Arguments.find(arg[0]) != m_Arguments.end())
        {
            std::cerr << "Parameter " << arg[0] << " was set before." << std::endl;
            exit(-1);
        }

        m_Arguments.insert(std::make_pair(arg[0], arg[1]));
    }
}

std::unordered_set<std::string> Engine::Settings::s_AvailableArguments =
{ ""
};
