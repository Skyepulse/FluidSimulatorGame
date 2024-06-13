#include "CameraController.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"
#include "../Event/KeyCode.h"

CameraController::CameraController(float aspectRatio, float zoomLevel, float cameraSpeed, Bound bound)
    : m_AspectRatio(aspectRatio), m_ZoomLevel(zoomLevel), m_CameraSpeed(cameraSpeed), m_UseLimitBound(true), m_LimitBound(bound)
{
    m_Camera = std::make_shared<Camera>(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

CameraController::CameraController(float aspectRatio, float zoomLevel, float cameraSpeed)
    : m_AspectRatio(aspectRatio), m_ZoomLevel(zoomLevel), m_CameraSpeed(cameraSpeed), m_UseLimitBound(false), m_LimitBound({ glm::vec2(0.0), glm::vec2(0.0) })
{
    m_Camera = std::make_shared<Camera>(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

CameraController::~CameraController()
{
}

bool CameraController::OnEvent(Event & e)
{
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<WindowResizedEvent>(CORE_BIND_EVENT_METHOD(CameraController, OnWindowResized));
  dispatcher.Dispatch<MouseScrolledEvent>(CORE_BIND_EVENT_METHOD(CameraController, OnMouseScrolled));
  dispatcher.Dispatch<KeyPressedEvent>(CORE_BIND_EVENT_METHOD(CameraController, OnKeyPressed));

  return true;
}

void CameraController::LimitCameraMovementInBound(bool enabled)
{
    m_UseLimitBound = enabled;
}

void CameraController::SetCameraMovementBound(const Bound bound)
{
    m_LimitBound = bound;
}

bool CameraController::OnKeyPressed(KeyPressedEvent &e)
{
  glm::vec2 translation(0.0f);
  switch (e.GetKey())
  {
  case CORE_KEY_W:
    translation += glm::vec2(0.0f, m_CameraSpeed);
    break;
  case CORE_KEY_S:
    translation -= glm::vec2(0.0f, m_CameraSpeed);
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

  Bound newCameraBound = m_Camera->GetBound();
  newCameraBound.MaxCorner += translation;
  newCameraBound.MinCorner += translation;

  if (m_UseLimitBound && !m_LimitBound.Includes(newCameraBound))
      return false;

  m_Camera->Translate(translation);
  return true;
}

bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    m_ZoomLevel -= (float)e.GetYOffset();
    m_Camera->SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    CORE_DEBUG("Received event")
    return true;
}

bool CameraController::OnWindowResized(WindowResizedEvent& e)
{
    glm::vec2 size = e.GetSize();
    m_AspectRatio = size.x / size.y;

    m_Camera->SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

    return true;
}
