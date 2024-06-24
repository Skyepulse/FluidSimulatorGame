#pragma once

#include "Transform.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"

#include "../Utils/Bounds.h"

#include "glm/glm.hpp"
#include <iostream>

class Camera
{
public:
  Camera(float left, float right, float bottom, float top, float nearPlane = -1.0f, float farPlane = 1.0f);
  ~Camera();

  void SetProjection(float left, float right, float bottom, float top, float nearPlane = -1.0f, float farPlane = 1.0f);

  const glm::mat4& GetVPMatrix() { m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix; return m_VPMatrix; }
  void Translate(const glm::vec2& translation);
  void SetPosition(const glm::vec2& position);
private:
  void RecalculateViewMatrix() { m_ViewMatrix = glm::inverse(m_Transform2D.GetModelMatrix()); }
private:
  glm::mat4 m_ViewMatrix, m_ProjectionMatrix, m_VPMatrix;
  Transform2D m_Transform2D;
};