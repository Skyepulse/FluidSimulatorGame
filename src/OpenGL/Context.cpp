#include "Context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
	//CORE_ASSERT(windowHandle, "Window handle is null!");
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

	std::cout << " Infos:" << std::endl;
	std::cout << "	- Vendor:   " << vendor << std::endl;
	std::cout << "	- Renderer: " << renderer << std::endl;
	std::cout << "	- Version:  " << version << std::endl;
	std::cout << "  - OpenGL version : " << glVersion << std::endl;
	
	//CORE_INFO(" Infos:");
	//CORE_INFO("	- Vendor:   {0}", vendor);
	//CORE_INFO("	- Renderer: {0}", renderer);
	//CORE_INFO("	- Version:  {0}", version);

	int versionMajor;
	int versionMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

	std::cout << "OpenGL major version : " << versionMajor << std::endl;
	std::cout << "OpenGL minor version : " << versionMinor << std::endl;

	//CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires at least  version 4.5!");
}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_WindowHandle);
}