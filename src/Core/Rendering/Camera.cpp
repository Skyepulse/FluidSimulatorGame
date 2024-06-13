#include "Camera.h"

#include <glm/ext.hpp>

Camera::Camera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
  // Translate camera so that it can render object with z = 0
  m_Transform2D.Translate(glm::vec3(0.0, 0.0f, 1.0f));

  SetProjection(left, right, bottom, top, nearPlane, farPlane);
}

Camera::~Camera()
{
}

void Camera::SetProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	m_Bound = Bound(glm::vec2(left, bottom), glm::vec2(right, top));
	CORE_DEBUG("Bound : Max({}, {}), Min({}, {})", m_Bound.MaxCorner.x, m_Bound.MaxCorner.y, m_Bound.MinCorner.x, m_Bound.MinCorner.y)

	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	RecalculateViewMatrix();
}

void Camera::Translate(const glm::vec2& translation)
{
	m_Bound.MaxCorner += translation;
	m_Bound.MinCorner += translation;

	m_Transform2D.Translate2D(translation); 
	RecalculateViewMatrix();
}
