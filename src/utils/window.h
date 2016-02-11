#pragma once
#include <string>
#include <functional>
#include "keys.h"

namespace Utils
{

	/**
	 * @brief Small utility to create and handle windows platform independently
	 */
	class Window
	{
	public:

		/**
		 * @brief Events that can occur
		 */
		enum EEvent
		{
			E_Closed,           // Window was closed
			E_Resized,			// Window was resized
			E_KeyEvent			// Key was pressed/released
		};

		/**
		 * @brief Key actions
		 */
		enum EKeyAction
		{
			EA_Pressed = 1,
			EA_Released = 0,
			EA_Repeated = 2
		};

		/**
		 * @brief Information about all possible events 
		 */
		struct Event
		{
			Event(){}
			Event(EEvent evt)
			{
				EventType = evt;
			}

			struct
			{
				// Scroll offsets
				double xOffset; 
				double yOffset;
			}ScrollEvent;

			struct
			{
				EKeyAction action;
				EKey key; // virtual key
				int scancode; // System specific key scancode
				int mods; // GLFW-like bitfield for key-flags

			}KeyboardEvent;

			EEvent EventType;
		};

		/**
		 * @brief Creates the window using the given parameters. The window will stay open as long as the object exists
		 */
		Window(unsigned int topX, unsigned int topY, unsigned int bottomX, unsigned int bottomY, const std::string& title);

		/**
		 * @brief Closes the window 
		 */
		virtual ~Window();

		/**
		 * @brief Polls the window for events and calls the given callback function for each event
		 */
        virtual void pollEvent(const std::function<void(Event)>& callback) = 0;

		/**
		 * @brief Returns the OS-Specific handle to this window as a void*
		 */
		virtual void* getNativeHandle() = 0;

		/**
		 * @brief Sets the title of the window
		 */
		virtual void setWindowTitle(const std::string& title){}
    };
}
