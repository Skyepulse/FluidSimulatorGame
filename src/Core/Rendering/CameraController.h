#pragma once

#include "Camera.h"
#include "Transform.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"

class CameraController
{
public:
  CameraController(float aspectRatio, float zoomLevel, float cameraSpeed);
  ~CameraController();

  bool OnEvent(Event& e);

  std::shared_ptr<Camera> GetCamera() const { return m_Camera; }
private:
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizedEvent& e);
private:
  std::shared_ptr<Camera> m_Camera;
  float m_CameraSpeed;
  float m_AspectRatio;
  float m_ZoomLevel;
};