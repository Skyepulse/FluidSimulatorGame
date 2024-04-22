#pragma once

struct GLFWwindow;

class OpenGLContext
{
public:
	OpenGLContext(GLFWwindow* windowHandle);
	~OpenGLContext();

	void Init();
	void SwapBuffers();
private:
	GLFWwindow* m_WindowHandle;
};

