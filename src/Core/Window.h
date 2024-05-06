#pragma once

#include "OpenGL/Context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

/// @brief GLFWwindow abstraction
class Window
{
public:
  Window();
  ~Window();

  GLFWwindow* Get() const { return m_Window; }
  
  bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
  void OnUpdate() const { m_Context->SwapBuffers(); glfwPollEvents(); }
private:
  void Init();

  GLFWwindow* m_Window;
  static bool isGLFWInit;
  std::shared_ptr<OpenGLContext> m_Context;
};