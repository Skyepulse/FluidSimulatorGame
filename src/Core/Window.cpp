#include "Window.h"

#include "Core.h"

bool Window::isGLFWInit = false;

Window::Window()
{
  CORE_TRACE("Creating new GLFW Window");
  if(!isGLFWInit)
    Init();
}

Window::~Window()
{
  CORE_TRACE("Destroying Window pointer");

 	glfwDestroyWindow(m_Window); // clear m_Window
  CORE_ASSERT(m_Window == nullptr, "Window is not cleared");
}

void Window::Init()
{
  CORE_TRACE("Initializing GLFW");

  glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(800, 600, "GlassOverFlow", NULL, NULL);
	if (m_Window == NULL) {
		glfwTerminate();
		CORE_ASSERT(false, "Failed to create Window");
	}

  isGLFWInit = true;
}
