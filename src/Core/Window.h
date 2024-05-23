#pragma once

#include "OpenGL/Context.h"

#include "Event/Event.h"
#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <functional>

#define EventCallback std::function<void(Event&)>

struct WindowProperties
{
  uint32_t Width, Height;
};

/// @brief GLFWwindow abstraction
class Window
{
public:
  struct WindowData
  {
    uint32_t Width, Height;
    EventCallback eventCallback;
  };

  Window(WindowProperties windowProps, EventCallback callback);
  ~Window();

  GLFWwindow* Get() const { return m_Window; }
  uint32_t GetWidth() const { return m_Data->Width; } 
  uint32_t GetHeight() const { return m_Data->Height; } 
  
  bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
  void OnUpdate() const { m_Context->SwapBuffers(); glfwPollEvents(); }

  void EnableVSync(bool enabled);
private:
  void Init();

private:
  GLFWwindow* m_Window;
  static bool isGLFWInit;
  std::shared_ptr<OpenGLContext> m_Context;

  WindowData* m_Data;
};