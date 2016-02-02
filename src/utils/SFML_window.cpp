#include "SFML_window.h"
#include <SFML/Graphics.hpp>

using namespace Utils;

/**
* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
*/
SFML_Window::SFML_Window(unsigned int topX, unsigned int topY, unsigned int bottomX, unsigned int bottomY, const std::string& title) 
	: Window(topX, topY, bottomX, bottomY, title),
	m_WindowHandle(sf::VideoMode(bottomX - topX, bottomY - topY), title)
{
}

/**
* @brief Closes the window 
*/
SFML_Window::~SFML_Window()
{

}

/**
* @brief Polls the window for events and calls the given callback function for each event
*/
void SFML_Window::pollEvent(const std::function<void(EEvent)>& callback)
{
	sf::Event e;
	m_WindowHandle.pollEvent(e);

	switch(e.type)
	{
	case sf::Event::Resized:
		callback(EEvent::E_Resized);
		break;

	case sf::Event::Closed:
		callback(EEvent::E_Closed);
		break;
	}
}

/**
* @brief Returns the OS-Specific handle to this window as a void*
*/
void* SFML_Window::getNativeHandle()
{
	return (void*)m_WindowHandle.getSystemHandle();
}
