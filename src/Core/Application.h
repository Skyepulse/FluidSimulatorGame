#pragma once

#include "Window.h"
#include "Log.h"
#include "Event/Event.h"
#include "Event/ApplicationEvent.h"

#include <queue>

class Application 
{
  friend class EventEnqueuer;
public:
  static Application* Create();

  void Start();
  void OnEvent(Event& e);

  bool OnKeyPressed(KeyPressedEvent& e);
  bool OnKeyReleased(KeyReleasedEvent& e);
  bool OnKeyTyped(KeyTypedEvent& e);

  bool OnMousePressed(MousePressedEvent& e);
  bool OnMouseReleased(MouseReleasedEvent& e);
  bool OnMouseTyped(MouseTypedEvent& e);

  ~Application();
private:
  Application();

private:
  std::shared_ptr<Window> m_Window;
};