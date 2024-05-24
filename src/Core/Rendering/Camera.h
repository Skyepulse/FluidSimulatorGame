#pragma once

#include "Transform.h"

#include "glm/glm.hpp"
#include <iostream>

#include "../Log.h"

class Camera
{
public:
  Camera(float left, float right, float bottom, float top, float nearPlane = 0.1f, float farPlane = 100.0f);
  ~Camera();
  const glm::mat4& GetVPMatrix() { m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix; return m_VPMatrix; }

  void Translate(const glm::vec2& translation) { m_Transform2D.Translate2D(translation); RecalculateViewMatrix(); }

  const glm::vec2& GetSize() const { return m_Size; }
  float GetWidth() const { return m_Size.x; }
  float GetHeight() const { return m_Size.y; }
  const glm::vec2& GetOrigin() const { return m_Origin; }
private:
  void RecalculateViewMatrix() { m_ViewMatrix = glm::inverse(m_Transform2D.GetModelMatrix()); }
private:
  glm::mat4 m_ViewMatrix, m_ProjectionMatrix, m_VPMatrix;
  Transform2D m_Transform2D;
  glm::vec2 m_Size;
  glm::vec2 m_Origin;
};