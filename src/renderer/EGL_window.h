#pragma once
#include "window.h"

struct EGLwindow;

#ifdef __ANDROID__

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <android/log.h>
#include <android_native_app_glue.h>

struct android_app;
namespace Renderer
{
	/**
	* @brief Small utility to create and handle windows platform independently
	*/	
	class EGL_Window : public Window
	{
	public:
		/**
		* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
		*/
		EGL_Window(const std::string& title, android_app* appHandle);

		/**
		* @brief Closes the window 
		*/
		~EGL_Window();

		/**
		* @brief Polls the window for events and calls the given callback function for each event
		*/
		virtual void pollEvent(const std::function<void(Event)>& callback) override;

		/**
		* @brief Returns the OS-Specific handle to this window as a void*
		*/
		virtual void* getNativeHandle() override;

		/**
		* @brief Returns the EGL-Surface of this window
		*/
		void* getSurface();

		/**
		* @brief Sets the title of the window
		*/
		virtual void setWindowTitle(const std::string& title) override;

		/**
		* @brief Gets the current keystate
		*/
		virtual bool getKeyPressed(Utils::EKey key) override {return m_KeyPresses[key];}

		/**
		* @brief Semi-Private function to set a key.
		*/
		void setKeyPressed(Utils::EKey key, bool value){m_KeyPresses[key] = value;}

		/**
		* @brief Returns a virtual thumbstick-position
		*/
		virtual Math::float2 getVirtualThumbstickDirection(int idx){return m_ThumbstickPosition[idx];}
	private:

		/**
		* @brief state of all (simulated) keys
		*/
		bool m_KeyPresses[Utils::KEY_LAST];

		/**
		 * @brief Initializes the EGL-Resources
		 */
		void initDisplay();

		/**
		* @brief Window-Handle of this object 
		*/
		android_app* m_pAppHandle;

		/**
		 * @brief EGL specific resources 
		 */
		EGLDisplay m_Display;
		EGLSurface m_Surface;
		EGLContext m_Context;
		int32_t m_Width;
		int32_t m_Height;

		/**
		 * @brief Virtual onscreen-thumbstick
		 */
		Math::float2 m_ThumbstickPosition[2];
	};
}

#endif