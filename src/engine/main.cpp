#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
#ifdef ZE_GAME
    std::cout << "Hello OpenZE!" << std::endl;
#elif ZE_SERVER
    std::cout << "Hello OZerver!" << std::endl;
#endif

#ifdef ZE_DEBUG
    std::cerr << "Da is was kaputt" << std::endl;
#endif
    return 0;
}
