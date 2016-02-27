#include "GLFW_window.h"
#include <GLFW/glfw3.h>
#include "logger.h"
#include <string.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

using namespace Utils;

/**
* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
*/
GLFW_Window::GLFW_Window(unsigned int topX, unsigned int topY, unsigned int bottomX, unsigned int bottomY, const std::string& title) :
    Window(topX, topY, bottomX, bottomY, title),
    m_InMenu(false)
{
	// Init key-array
	memset(m_KeyPresses, 0, sizeof(m_KeyPresses));

	/* Initialize the library */
	if(!glfwInit())
	{
		LogError() << "glfwInit() failed!";
		return;
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
	m_pWindowHandle = glfwCreateWindow(1280, 720, "--- Test ---", nullptr, nullptr);

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
		e.KeyboardEvent.key = (EKey)key;
		e.KeyboardEvent.scancode = scancode;

		// Set key-state (Ignore repeat-events)
		if(action == EKeyAction::EA_Pressed)
			hlp->thisptr->setKeyPressed((EKey)key, true);
		else if(action == EKeyAction::EA_Released)
			hlp->thisptr->setKeyPressed((EKey)key, false);

		fn(e);
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
	//return m_pWindowHandle;
	// TODO: OpenGL under Windows is broken because of this!
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
