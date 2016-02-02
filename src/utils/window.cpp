#include "window.h"
#include <SFML\Graphics.hpp>

using namespace Utils;

/**
* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
*/
Window::Window(unsigned int topX, unsigned int topY, unsigned int bottomX, unsigned int bottomY, const std::string& title)
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
}

/**
* @brief Closes the window 
*/
Window::~Window()
{

}