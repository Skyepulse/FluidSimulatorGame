#include "Camera.h"

#include <glm/ext.hpp>

Camera::Camera(float left, float right, float bottom, float top)
{
  m_Origin = glm::vec2(left, bottom);
  m_Size = glm::vec2(right - left, top - bottom);

  // Translate camera so that it can render object with z = 0
  m_Transform2D.Translate(glm::vec3(0.0, 0.0f, 1.0f));

  m_ProjectionMatrix = glm::ortho(left, right, bottom, top, 0.1f, 100.0f);
  m_ViewMatrix = glm::inverse(m_Transform2D.GetModelMatrix());
}

Camera::~Camera()
{
}
