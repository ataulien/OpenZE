#include "GLFW_window.h"

#ifndef RAPI_USE_GLES3
#define RAPI_USE_GLFW
#endif

#ifdef RAPI_USE_GLFW

#include <RDevice.h>
#include <GLFW/glfw3.h>
#include "utils/logger.h"
#include <string.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

using namespace Renderer;

/**
* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
*/
GLFW_Window::GLFW_Window(unsigned int topX, unsigned int topY, unsigned int width, unsigned int height, const std::string& title) :
    Window(topX, topY, width, height, title),
    m_InMenu(false),
	m_IsFullscreen(false)
{
	// Init key-array
	memset(m_KeyPresses, 0, sizeof(m_KeyPresses));

	m_TopX = topX;
	m_TopY = topY;
	m_Width = width;
	m_Height = height;

	/* Initialize the library */
	if(!glfwInit())
	{
		LogError() << "glfwInit() failed!";
		return;
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	m_pWindowHandle = glfwCreateWindow(width, height, "--- Test ---", nullptr, nullptr);

	// Move window
	glfwSetWindowPos(m_pWindowHandle, m_TopX, m_TopY);

	// Turn off vsync
	glfwSwapInterval(0);

	if(!m_pWindowHandle)
	{
		LogError() << "glfwCreateWindow() failed!";
		glfwTerminate();
		return;
	}

    //glfwSetInputMode(m_pWindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetCursorPosCallback(m_pWindowHandle, [](GLFWwindow* pWindow, double posX, double posY){(void)pWindow; LogInfo() << posX << " " << posY;});
}

/**
* @brief Closes the window 
*/
GLFW_Window::~GLFW_Window()
{
	// We only support one window at a time using glfw
	glfwTerminate();
}

/**
* @brief Switches to fullscreen/windowed
*/
void GLFW_Window::switchMode(bool fullscreen)
{
	// Get the desktop resolution.
	int count;
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	m_IsFullscreen = fullscreen;

	if(fullscreen) {
		// Set window size for "fullscreen windowed" mode to the desktop resolution.
		glfwSetWindowSize(m_pWindowHandle, mode->width, mode->height);
		// Move window to the upper left corner.
		glfwSetWindowPos(m_pWindowHandle, 0, 0);
	}
	else {
		// Use start-up values for "windowed" mode.
		glfwSetWindowSize(m_pWindowHandle, m_Width, m_Height);
		glfwSetWindowPos(m_pWindowHandle, m_TopX, m_TopY);
	}

}

/**
* @brief Polls the window for events and calls the given callback function for each event
*/
void GLFW_Window::pollEvent(const std::function<void(Event)>& callback)
{
	// Set our callback function as userdata so we can access it from the glfw callbacks

	struct hlpStruct
	{
		std::function<void(Event)> fn;
		GLFW_Window* thisptr;
	}userData;

	userData.fn = callback;
	userData.thisptr = this;

	glfwSetWindowUserPointer(m_pWindowHandle, &userData);

	// Assign callbacks
	glfwSetKeyCallback(m_pWindowHandle, [](GLFWwindow* wnd, int key, int scancode, int action, int mods)
	{
        (void)mods;
		hlpStruct* hlp = reinterpret_cast<hlpStruct*>(glfwGetWindowUserPointer(wnd));

		// Get our callback funktion back
		std::function<void(Event)> fn = hlp->fn;

		Event e(EEvent::E_KeyEvent);
		e.KeyboardEvent.action = (EKeyAction)action;
		e.KeyboardEvent.key = (Utils::EKey)key;
		e.KeyboardEvent.scancode = scancode;

		// Set key-state (Ignore repeat-events)
		if(action == EKeyAction::EA_Pressed)
			hlp->thisptr->setKeyPressed((Utils::EKey)key, true);
		else if(action == EKeyAction::EA_Released)
			hlp->thisptr->setKeyPressed((Utils::EKey)key, false);

		fn(e);
	});
	
	glfwSetWindowSizeCallback(m_pWindowHandle, [](GLFWwindow* wnd, int width, int height)
	{
		hlpStruct* hlp = reinterpret_cast<hlpStruct*>(glfwGetWindowUserPointer(wnd));

		Event e(EEvent::E_Resized);
		e.ResizeEvent.width = width;
		e.ResizeEvent.height = height;

		hlp->fn(e);
	});

	glfwPollEvents();

	glfwSetWindowUserPointer(m_pWindowHandle, nullptr);
	glfwSetKeyCallback(m_pWindowHandle, nullptr);

    //TODO: while?
	if(glfwWindowShouldClose(m_pWindowHandle))
		callback(EEvent::E_Closed);

	// TODO: Resize event

	/*sf::Event e;
	m_WindowHandle.pollEvent(e);

	switch(e.type)
	{
	case sf::Event::Resized:
		callback(EEvent::E_Resized);
		break;

	case sf::Event::Closed:
		callback(EEvent::E_Closed);
		break;
	}*/
}

/**
* @brief Returns the GLFW-handle of this window
*/
GLFWwindow* GLFW_Window::getGLFWwindow()
{
	return m_pWindowHandle;
}

/**
* @brief Returns the OS-Specific handle to this window as a void*
*/
void* GLFW_Window::getNativeHandle()
{
#if defined(WIN32) || defined(_WIN32)
	return glfwGetWin32Window(m_pWindowHandle);
#else
    return m_pWindowHandle;
#endif
}


/**
* @brief Sets the title of the window
*/
void GLFW_Window::setWindowTitle(const std::string& title)
{
    glfwSetWindowTitle(m_pWindowHandle, title.c_str());
}

#endif
