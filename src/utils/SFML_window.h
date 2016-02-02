#pragma once
#include "window.h"
#include <SFML\Graphics.hpp>

namespace Utils
{
	/**
	 * @brief Small utility to create and handle windows platform independently
	 */
	class SFML_Window : public Window
	{
	public:
		/**
		* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
		*/
		SFML_Window(unsigned int topX, unsigned int topY, unsigned int bottomX, unsigned int bottomY, const std::string& title);

		/**
		* @brief Closes the window 
		*/
		~SFML_Window();

		/**
		* @brief Polls the window for events and calls the given callback function for each event
		*/
		virtual void pollEvent(const std::function<void(EEvent)>& callback) override;

		/**
		* @brief Returns the OS-Specific handle to this window as a void*
		*/
		virtual void* getNativeHandle() override;
	private:
		/**
		 * @brief Window-Handle of this object 
		 */
		sf::RenderWindow m_WindowHandle;
	};
}