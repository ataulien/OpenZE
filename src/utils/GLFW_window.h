#pragma once
#include "window.h"

struct GLFWwindow;

namespace Utils
{
	/**
	* @brief Small utility to create and handle windows platform independently
	*/
	class GLFW_Window : public Window
	{
	public:
		/**
		* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
		*/
		GLFW_Window(unsigned int topX, unsigned int topY, unsigned int bottomX, unsigned int bottomY, const std::string& title);

		/**
		* @brief Closes the window 
		*/
		~GLFW_Window();

		/**
		* @brief Polls the window for events and calls the given callback function for each event
		*/
		virtual void pollEvent(const std::function<void(Event)>& callback) override;

		/**
		* @brief Returns the OS-Specific handle to this window as a void*
		*/
		virtual void* getNativeHandle() override;

		/**
		* @brief Returns the GLFW-handle of this window
		*/
		GLFWwindow* getGLFWwindow();

		/**
		* @brief Sets the title of the window
		*/
		virtual void setWindowTitle(const std::string& title) override;

		/**
		* @brief Gets the current keystate
		*/
		virtual bool getKeyPressed(EKey key) override {return m_KeyPresses[key];};

		/**
		 * @brief Semi-Private function to set a key.
		 */
		void setKeyPressed(EKey key, bool value){m_KeyPresses[key] = value;}
	private:

		/**
		* @brief Window-Handle of this object 
		*/
		GLFWwindow* m_pWindowHandle;

		/**
		 * @brief state of all keys
		 */
		bool m_KeyPresses[KEY_LAST];
	};
}
