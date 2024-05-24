#include "CameraController.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"
#include "../Event/KeyCode.h"

CameraController::CameraController(std::shared_ptr<Camera> camera, float cameraSpeed) : m_Camera(camera), m_CameraSpeed(cameraSpeed)
{
}

CameraController::~CameraController()
{
}

void CameraController::OnEvent(Event & e)
{
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<KeyPressedEvent>(CORE_BIND_EVENT_METHOD(CameraController, OnKeyPressed));
}

bool CameraController::OnKeyPressed(KeyPressedEvent &e)
{
  glm::vec2 translation(0.0f);
  switch (e.GetKey())
  {
  case CORE_KEY_W:
    translation -= glm::vec2(0.0f, m_CameraSpeed);
    break;
  case CORE_KEY_S:
    translation += glm::vec2(0.0f, m_CameraSpeed);
    break;
  case CORE_KEY_A:
    translation -= glm::vec2(m_CameraSpeed, 0.0f);
    break;
  case CORE_KEY_D:
    translation += glm::vec2(m_CameraSpeed, 0.0f);
    break;
  default:
    return false;
  }

  m_Camera->Translate(translation);
  return true;
}