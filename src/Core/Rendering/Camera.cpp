#include "Camera.h"

#include <glm/ext.hpp>

Camera::Camera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
  m_Origin = glm::vec2(left, bottom);
  m_Size = glm::vec2(right - left, top - bottom);

  // Translate camera so that it can render object with z = 0
  m_Transform2D.Translate(glm::vec3(0.0, 0.0f, 1.0f));

  SetProjection(left, right, bottom, top, nearPlane, farPlane);
}

Camera::~Camera()
{
}

void Camera::SetProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	RecalculateViewMatrix();
}
