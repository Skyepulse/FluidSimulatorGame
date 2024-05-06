#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

/// @brief OpenGL context interactions
class OpenGLContext
{
public:
	OpenGLContext(GLFWwindow* windowHandle);
	~OpenGLContext() { }

	void Init();
	void SwapBuffers();
private:
	GLFWwindow* m_WindowHandle;
};

