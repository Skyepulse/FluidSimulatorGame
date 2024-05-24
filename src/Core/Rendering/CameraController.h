#pragma once

#include "Camera.h"
#include "Transform.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"

class CameraController
{
public:
  CameraController(std::shared_ptr<Camera> camera, float cameraSpeed);
  ~CameraController();

  void OnEvent(Event& e);
private:
  bool OnKeyPressed(KeyPressedEvent& e);
private:
  std::shared_ptr<Camera> m_Camera;
  float m_CameraSpeed;
};