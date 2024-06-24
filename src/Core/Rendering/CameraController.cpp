#include "CameraController.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"
#include "../Event/KeyCode.h"

CameraController::CameraController(float aspectRatio, float zoomLevel, float cameraSpeed, Bound bound)
    : m_AspectRatio(aspectRatio), m_ZoomLevel(zoomLevel), m_CameraSpeed(cameraSpeed),m_LimitBound(bound), m_CameraPosition(glm::vec2(0))
{
    m_Camera = std::make_shared<Camera>(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

CameraController::CameraController(float aspectRatio, float zoomLevel, float cameraSpeed)
    : m_AspectRatio(aspectRatio), m_ZoomLevel(zoomLevel), m_CameraSpeed(cameraSpeed), m_LimitBound(glm::vec2(0.0), glm::vec2(0.0)), m_CameraPosition(glm::vec2(0))
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
  dispatcher.Dispatch<KeyPressedEvent>(CORE_BIND_EVENT_METHOD(CameraController, OnKeyPressed));

  return true;
}

void CameraController::SetCameraMovementBound(const Bound bound)
{
    m_LimitBound = bound;
}

/// <summary>
/// Set the camera position.
/// Have to be called after setting the camera bound !
/// </summary>
/// <param name="position"></param>
void CameraController::SetCameraPosition(const glm::vec2& position)
{
    CORE_DEBUG("Camera pos : {}, {}", position.x, position.y);

    if (!m_LimitBound.InBound(position))
        return;

    m_CameraPosition = position;
    m_Camera->SetPosition(position);
}

void CameraController::AdaptZoomLevel(const Bound& levelBound)
{
    m_ZoomLevel = (levelBound.MaxCorner.y + levelBound.MinCorner.y) / 2.0f;
    m_Camera->SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
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

  glm::vec2 newCameraPosition = m_CameraPosition + translation;

  if (!m_LimitBound.InBound(newCameraPosition))
      return false;

  m_CameraPosition = newCameraPosition;
  m_Camera->Translate(translation);
  return true;
}

bool CameraController::OnWindowResized(WindowResizedEvent& e)
{
    glm::vec2 size = e.GetSize();
    m_AspectRatio = size.x / size.y;

    m_Camera->SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
   
    return true;
}

glm::vec2 CameraController::getCenterBound(){
  return (m_LimitBound.MaxCorner + m_LimitBound.MinCorner)/2.0f;
}
