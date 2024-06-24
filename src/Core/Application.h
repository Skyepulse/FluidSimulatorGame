#pragma once

#include "Window.h"
#include "Log.h"
#include "Layer.h"

#include "Rendering/CameraController.h"
#include "Scene/Rectangle.h"

#include "Event/Event.h"
#include "Event/ApplicationEvent.h"

#include "UI/UserInterface.h"

enum class GameState {
	LOSE = 2,
	WIN = 3,
	PAUSED = 1,
	RUNNING = 0
};

class Application 
{
public:
  static Application* Create(); // Must be implemented in the Game Application

  void Start();
  void OnEvent(Event& e);
  bool OnWindowResized(WindowResizedEvent& e);

  void PushOverlay(Layer* overlay) { m_Layers.PushOverlay(overlay); }
  void PushLayer(Layer* layer) { m_Layers.PushLayer(layer); }
  void PopLayer(Layer* layer ) { m_Layers.PopLayer(layer); }

  virtual void loadGame(int index) = 0;
  virtual void unloadGame() = 0;
  virtual void restartGame() = 0;
  virtual float getGameTime() = 0;
  virtual void pauseGame() = 0;
  virtual void resumeGame() = 0;
  virtual void loadNextGame() = 0;

  virtual GameState getGameState() = 0;
  static inline Application* Get() { return s_Instance; }
  std::shared_ptr<Window> GetWindow() { return m_Window; }
  
  std::shared_ptr<CameraController> GetCameraController() const { return m_Controller; }

  std::shared_ptr<UserInterface> GetUI() const { return userInterface; }

  ~Application();
protected:
  Application();
  std::shared_ptr<UserInterface> userInterface;
  void setBgSize();

private:
  static Application* s_Instance; 
  std::shared_ptr<Window> m_Window;
  LayerStack m_Layers;
  bool m_Minimized = false;

  std::shared_ptr<CameraController> m_Controller;

  std::shared_ptr<Shape> background;
  std::shared_ptr<Texture2D> backgroundTex;
};