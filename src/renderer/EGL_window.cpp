#include "EGL_window.h"
#include "utils/logger.h"
#include <string.h>

#ifdef __ANDROID__

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

using namespace Renderer;

/**
* @brief Creates the window using the given parameters. The window will stay open as long as the object exists
*/
EGL_Window::EGL_Window(const std::string& title, android_app* appHandle) :
    Window(0, 0, 0, 0, title),
	m_pAppHandle(appHandle),
	m_Surface(0)
{ 
	LogInfo() << "Creating EGL-Window...";
	m_ThumbstickPosition[0] = Math::float2(0,0);
	m_ThumbstickPosition[1] = Math::float2(0,0);
}

/**
* @brief Closes the window 
*/
EGL_Window::~EGL_Window()
{
	eglMakeCurrent(m_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	if (m_Context != EGL_NO_CONTEXT) {
		eglDestroyContext(m_Display, m_Context);
	}
	if (m_Surface != EGL_NO_SURFACE) {
		eglDestroySurface(m_Display, m_Surface);
	}
	eglTerminate(m_Display);
}

/**
* @brief Initializes the EGL-Resources
*/
void EGL_Window::initDisplay()
{
	/*
	* Here specify the attributes of the desired configuration.
	* Below, we select an EGLConfig with at least 8 bits per color
	* component compatible with on-screen windows
	*/
	const EGLint attribs[] = {
		// 32 bit color
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		// at least 24 bit depth
		EGL_DEPTH_SIZE, 24,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		// want opengl-es 2.x conformant CONTEXT
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, 
		EGL_NONE
	};
	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	LogInfo() << "Initializing EGL...";
	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	LogInfo() << "Choosing EGL-Config...";
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	LogInfo() << "Setting buffers geometry...";
	ANativeWindow_setBuffersGeometry(m_pAppHandle->window, 0, 0, format);

	LogInfo() << "Creating surface...";
	surface = eglCreateWindowSurface(display, config, m_pAppHandle->window, NULL);
	LogInfo() << "Surface created: " << surface;

	LogInfo() << "Creating context...";

	EGLint attribsES3[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
	context = eglCreateContext(display, config, NULL, attribsES3);
	if(context == EGL_NO_CONTEXT)
	{
		LogInfo() << "Failed to create OpenGL ES 3-Context, trying ES2...";

		//We failed to create the ES 3 context. Try ES 2
		EGLint attribsES2[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
		context = eglCreateContext(display, config, NULL, attribsES2);
		if(context == EGL_NO_CONTEXT)
		{
			//Something is wrong... Failed to create the ES2 context
			LogError() << "Failed to create a supported ES Context";
		}
	}


	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	LogInfo() << "Got surface of size: " << w << "x" << h;

	m_Display = display;
	m_Context = context;
	m_Surface = surface;
	m_Width = w;
	m_Height = h;

	// Initialize GL state.
	/*glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);

	glViewport(0, 0, w, h);*/
}

/**
* @brief Polls the window for events and calls the given callback function for each event
*/
void EGL_Window::pollEvent(const std::function<void(Event)>& callback)
{
	m_pAppHandle->userData = this;
	m_pAppHandle->onAppCmd = [](struct android_app* app, int32_t cmd){
			struct EGL_Window* window = (struct EGL_Window*)app->userData;
			switch (cmd) {
			case APP_CMD_SAVE_STATE:
				// The system has asked us to save our current state.  Do so.
				//window->m_pAppHandle->savedState = malloc(sizeof(saved_state));
				//*((struct saved_state*)window->m_pAppHandle->savedState) = engine->state;
				//window->m_pAppHandle->savedStateSize = sizeof(saved_state);
				break;
			case APP_CMD_INIT_WINDOW:
				LogInfo() << " ############################# INIT ###############################";
				// The window is being shown, get it ready.
				if (window->m_pAppHandle->window != NULL) {
					window->initDisplay();
				}
				break;
			case APP_CMD_TERM_WINDOW:
				// The window is being hidden or closed, clean it up.
				//engine_term_display(window);
				break;
			case APP_CMD_GAINED_FOCUS:
				break;
			case APP_CMD_LOST_FOCUS:
				break;
			}
		};

	m_pAppHandle->onInputEvent = [](struct android_app* app, AInputEvent* event) {
			struct EGL_Window* window = (struct EGL_Window*)app->userData;
			static int activePointerIdx;
			switch(AInputEvent_getSource(event))
			{
			case AINPUT_SOURCE_TOUCHSCREEN:
				int action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
				int pointer = (AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

				//LogInfo() << "Pointer: " << AMotionEvent_getPointerId(event, pointer);

				// Only need two pointers for now
				//if(pointer > 1)
				//	break;
				static int s_tumbLastFrameDown[2] = { 0,0 };

				switch(action){ 
				case AMOTION_EVENT_ACTION_MOVE:
				case AMOTION_EVENT_ACTION_DOWN:
				case AMOTION_EVENT_ACTION_POINTER_DOWN:
					{
						int pointerCount = AMotionEvent_getPointerCount(event);
						for(int i = 0; i < pointerCount; ++i)
						{
							int pointerIndex = i;
							int x = AMotionEvent_getX(event, pointerIndex);
							int y = AMotionEvent_getY(event, pointerIndex);

							const Math::float2 controlSize(window->m_Width / 16, window->m_Height / 16);


							// Movement
							int movementX = 0;
							int movementY = 0;
							int thumb = 0;

							if(x < window->m_Width / 2)
							{
								// Left stick
								movementX = window->m_Width / 6;
								movementY = window->m_Height - (window->m_Height / 6);
								thumb = 0;

								Math::float2 dir = Math::float2(static_cast<float>(x - movementX), static_cast<float>(y - movementY));

								//LogInfo() << "Thumb Source" << thumb << ": " << dir.toString();

								// Set max thumbstick position

								dir.x = dir.x > 0 ? std::min(controlSize.x, dir.x) : std::max(-controlSize.x, dir.x);
								dir.y = dir.y > 0 ? std::min(controlSize.y, dir.y) : std::max(-controlSize.y, dir.y);

								// Normalize
								dir.x /= controlSize.length();
								dir.y /= controlSize.length();

								//LogInfo() << "Thumb " << thumb << ": " << dir.toString();

								window->m_ThumbstickPosition[0] = dir;
							}
							else
							{
								// Right stick
								movementX = x - (window->m_Width - (window->m_Width / 6));
								movementY = y - (window->m_Width - (window->m_Height / 6));
								thumb = 1;

								static int s_lastX = movementX;
								static int s_lastY = movementY;						

								if(!s_tumbLastFrameDown[1])
								{
									s_lastX = movementX;
									s_lastY = movementY;
								}

								window->m_ThumbstickPosition[1] = 0.2f * Math::float2(static_cast<float>(movementX - s_lastX), static_cast<float>(movementY - s_lastY));

								s_lastX = movementX;
								s_lastY = movementY;

								//LogInfo() << "Thumb " << thumb << ": " << window->m_ThumbstickPosition[0].toString();

								s_tumbLastFrameDown[1] = 1;
							}

							
						}
					}
					return 1;
					break;
				case AMOTION_EVENT_ACTION_UP:
				case AMOTION_EVENT_ACTION_POINTER_UP:
					{
						int thumb = 0;
						int x = AMotionEvent_getX(event, pointer);
						int y = AMotionEvent_getY(event, pointer);

						if(x < window->m_Width / 2)				
							thumb = 0;				
						else				
							thumb = 1;
						
						s_tumbLastFrameDown[thumb] = 0;
						window->m_ThumbstickPosition[thumb] = Math::float2(0, 0);
					}
					return 1;
					break;
				}
				break;
			} // end switch

			return 0;
		};


	// Read all pending events.
	int ident;
	int events;
	struct android_poll_source* source;
	while ((ident=ALooper_pollAll(0, NULL, &events,
		(void**)&source)) >= 0) {

		// Process this event.
		if (source != NULL) {
			source->process(m_pAppHandle, source);
		}

		// Check if we are exiting.
		if (m_pAppHandle->destroyRequested != 0) {
			callback(EEvent::E_Closed);
			return;
		}
	}
}

/**
* @brief Returns the EGL-Surface of this window
*/
void* EGL_Window::getSurface()
{
	return m_Surface;
}

/**
* @brief Returns the OS-Specific handle to this window as a void*
*/
void* EGL_Window::getNativeHandle()
{
    return m_pAppHandle;
}


/**
* @brief Sets the title of the window
*/
void EGL_Window::setWindowTitle(const std::string& title)
{
    
}

#endif