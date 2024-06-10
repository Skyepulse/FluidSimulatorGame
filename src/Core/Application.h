#pragma once

#include "Window.h"
#include "Log.h"
#include "Layer.h"

#include "Rendering/CameraController.h"

#include "Event/Event.h"
#include "Event/ApplicationEvent.h"

#include "UI/UserInterface.h"

class Application 
{
public:
  static Application* Create(); // Must be implemented in the Game Application

  void Start();
  void OnEvent(Event& e);

  void PushOverlay(Layer* overlay) { m_Layers.PushOverlay(overlay); }
  void PushLayer(Layer* layer) { m_Layers.PushLayer(layer); }
  void PopLayer(Layer* layer ) { m_Layers.PopLayer(layer); }

  virtual void loadGame(int index) = 0;
  virtual void unloadGame() = 0;
  virtual void restartGame() = 0;

  static inline Application* Get() { return s_Instance; }
  std::shared_ptr<Window> GetWindow() { return m_Window; }

  ~Application();
protected:
  Application();

private:
  static Application* s_Instance; 
  std::shared_ptr<Window> m_Window;
  LayerStack m_Layers;

  // TEMP
  std::shared_ptr<CameraController> t_Controller;

  UserInterface userInterface;
};