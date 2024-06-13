#pragma once

#include "Camera.h"
#include "Transform.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"

class CameraController
{
public:
  CameraController(float aspectRatio, float zoomLevel, float cameraSpeed, Bound bound);
  CameraController(float aspectRatio, float zoomLevel, float cameraSpeed);
  ~CameraController();

  bool OnEvent(Event& e);
  void LimitCameraMovementInBound(bool enabled);
  void SetCameraMovementBound(const Bound bound);

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

 bool m_UseLimitBound;
 Bound m_LimitBound; // Limit the camera movement in the bound
};