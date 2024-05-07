#pragma once

#include "Transform.h"

#include "glm/glm.hpp"
#include <iostream>

class Camera
{
public:
  Camera(float left, float right, float bottom, float top);
  ~Camera();
  const glm::mat4& GetVPMatrix() { m_VPMatrix = m_ProjectionMatrix * m_ViewMatrix; return m_VPMatrix; }

  Transform2D& GetTransform2D() { return m_Transform2D; }
  Transform& GetTransform() { return dynamic_cast<Transform&>(m_Transform2D); }

  const glm::vec2& GetSize() const { return m_Size; }
  float GetWidth() const { return m_Size.x; }
  float GetHeight() const { return m_Size.y; }
  const glm::vec2& GetOrigin() const { return m_Origin; }
private:
  glm::mat4 m_ViewMatrix, m_ProjectionMatrix, m_VPMatrix;
  Transform2D m_Transform2D;
  glm::vec2 m_Size;
  glm::vec2 m_Origin;
};