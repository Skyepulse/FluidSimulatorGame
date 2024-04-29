#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

class Window
{
public:
  Window();
  ~Window();
  void MakeContextCurrent() const {	glfwMakeContextCurrent(m_Window); }
  const GLFWwindow* Get() const { return m_Window; }
  bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }
  void PollEvents() const { glfwPollEvents(); }
  void SwapBuffers() const { glfwSwapBuffers(m_Window); }
private:
  void Init();
  GLFWwindow* m_Window;
  static bool isGLFWInit;
};