#include "Window.h"

#include "Application.h"
#include "Core.h"

#include "Rendering/RendererCommand.h"

bool Window::isGLFWInit = false;

Window::Window(WindowProperties windowProps, EventCallback callback)
{ 
	m_Data = new WindowData;

	m_Data->Width = windowProps.Width;
	m_Data->Height = windowProps.Height;
	m_Data->eventCallback = callback;
	
	CORE_TRACE("Creating new GLFW Window");
	Init();

	// Store data in window
	glfwSetWindowUserPointer(m_Window, (void*)m_Data);
}

Window::~Window()
{
  CORE_TRACE("Destroying Window pointer");

	delete m_Data;
 	glfwDestroyWindow(m_Window); // clear m_Window
}

void Window::EnableVSync(bool enabled)
{
	if(enabled)
	{
		CORE_TRACE("Enabled VSync")
		glfwSwapInterval(1);
	}
	else
	{
		CORE_TRACE("Disabled VSync")
		glfwSwapInterval(0);
	}
}

void Window::Init()
{
	static bool isGLFWInit = false;

	if(isGLFWInit)
		return;

  CORE_TRACE("Initializing GLFW");

  glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(m_Data->Width, m_Data->Height, "GlassOverFlow", NULL, NULL);
	if (m_Window == NULL) {
		glfwTerminate();
		CORE_ASSERT(false, "Failed to create Window");
	}
  isGLFWInit = true;

	m_Context = std::make_shared<OpenGLContext>(m_Window);
	m_Context->Init();

	CORE_TRACE("Setting Event Callbacks");

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		static int previousKey = -1;
		static int previousAction = -1;

		WindowData* windowData = (WindowData*)glfwGetWindowUserPointer(window);

		switch(action)
		{
			case GLFW_PRESS:
			case GLFW_REPEAT:
			{
				KeyPressedEvent kp(key);
				windowData->eventCallback(kp);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent kr(key);
				windowData->eventCallback(kr);
				break;
			}
			default:
				CORE_ASSERT(false, "Unknown Key event action {}", action)
				break;
		}

		// Key typed event
		if(previousKey == key && (previousAction == GLFW_PRESS || previousAction == GLFW_REPEAT) && action == GLFW_RELEASE)
		{
			KeyTypedEvent e(key);
			windowData->eventCallback(e);
		}

		previousKey = key;
		previousAction = action;
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
	{
		WindowData* windowData = (WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent e(xpos, ypos);
		windowData->eventCallback(e);
	});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
		static int previousButton = -1;
		static int previousAction = -1;

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		WindowData* windowData = (WindowData*)glfwGetWindowUserPointer(window);

		switch(action)
		{
			case GLFW_PRESS:
			case GLFW_REPEAT:
			{
				MousePressedEvent mp(xpos, ypos, button);
				windowData->eventCallback(mp);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseReleasedEvent mr(xpos, ypos, button);
				windowData->eventCallback(mr);
				break;
			}
			default:
				CORE_ASSERT(false, "Unknown mouse input event action {}", action)
				break;
		}

		// Key typed event
		if(previousButton == button && (previousAction == GLFW_PRESS || previousAction == GLFW_REPEAT) && action == GLFW_RELEASE)
		{
			MouseTypedEvent e(xpos, ypos, button);
			windowData->eventCallback(e);
		}

		previousButton = button;
		previousAction = action;
	});

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
	{
		WindowData* windowData = (WindowData*)glfwGetWindowUserPointer(window);

		// TEMP ??
		RendererCommand::SetViewportSize(glm::ivec2(width, height));

		WindowResizedEvent e(width, height);
		windowData->eventCallback(e);
	});
}
