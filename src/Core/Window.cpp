#include "Window.h"

#include "Core.h"

bool Window::isGLFWInit = false;

Window::Window()
{
  CORE_TRACE("Creating new GLFW Window");
  Init();
}

Window::Window(std::shared_ptr<WindowProperties> properties)
{ 
	m_Width = properties->Width;
	m_Height = properties->Height;
	
	CORE_TRACE("Creating new GLFW Window");
	Init();
}

Window::~Window()
{
  CORE_TRACE("Destroying Window pointer");

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
		glfwSwapInterval(0);
		CORE_TRACE("Disabled VSync")
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

	m_Window = glfwCreateWindow(m_Width, m_Height, "GlassOverFlow", NULL, NULL);
	if (m_Window == NULL) {
		glfwTerminate();
		CORE_ASSERT(false, "Failed to create Window");
	}
  isGLFWInit = true;

	m_Context = std::make_shared<OpenGLContext>(m_Window);
	m_Context->Init();
}
