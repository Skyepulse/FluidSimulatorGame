#pragma once

#include "../Window.h"

#include <memory>

class OpenGLContext
{
public:
	OpenGLContext(const std::shared_ptr<Window> windowHandle);
	~OpenGLContext() { }

	void Init();
	void SwapBuffers();
private:
	const std::shared_ptr<Window> m_WindowHandle;
};

