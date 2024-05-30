#pragma once

#include "Window.h"
#include "Log.h"

#include "Rendering/CameraController.h"

#include "Event/Event.h"
#include "Event/ApplicationEvent.h"

// TEMP
#include "../Game/Game.h"

#include "UI/InGameUI.h"


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

  // TEMP
  std::shared_ptr<CameraController> t_Controller;
  std::shared_ptr<Game> t_Game;

  InGameUI inGameUI;
};