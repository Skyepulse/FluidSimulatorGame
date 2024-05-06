#pragma once

#include "OpenGL/Context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

struct WindowProperties
{
  uint32_t Width, Height;

};

/// @brief GLFWwindow abstraction
class Window
{
public:
  Window();
  Window(std::shared_ptr<WindowProperties> properties);
  ~Window();

  GLFWwindow* Get() const { return m_Window; }
  uint32_t GetWidth() const { return m_Width; } 
  uint32_t GetHeight() const { return m_Height; } 
  
  bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
  void OnUpdate() const { m_Context->SwapBuffers(); glfwPollEvents(); }

  void EnableVSync(bool enabled);
private:
  void Init();

private:
  GLFWwindow* m_Window;
  static bool isGLFWInit;
  std::shared_ptr<OpenGLContext> m_Context;
  uint32_t m_Width = 800, m_Height = 600;
};