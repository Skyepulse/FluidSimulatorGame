#include "Context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "../Core/Log.h"

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
	//CORE_ASSERT(windowHandle, "Window handle is null!");
}

OpenGLContext::~OpenGLContext()
{
	glfwDestroyWindow(m_WindowHandle);
}

void OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_WindowHandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//CORE_ASSERT(status, "Failed to initialize Glad!");

	const char* vendor = (const char*)glGetString(GL_VENDOR);
	const char* renderer = (const char*)glGetString(GL_RENDERER);
	const char* version = (const char*)glGetString(GL_VERSION);
	const char* glVersion = (const char*)glGetString(GL_VERSION);

	CORE_INFO(" Infos:");
	CORE_INFO("	- Vendor:   {0}", vendor);
	CORE_INFO("	- Renderer: {0}", renderer);
	CORE_INFO("	- Version:  {0}", version);

	//CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires at least  version 4.5!");
}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_WindowHandle);
}