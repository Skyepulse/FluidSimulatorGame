#pragma once

#include "Window.h"

class Application 
{
public:
  static Application* Create();

  void Start();
  ~Application();
private:
  Application();
private:
  std::shared_ptr<Window> m_Window;
};