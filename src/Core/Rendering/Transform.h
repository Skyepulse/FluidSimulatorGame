#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
  Transform() : m_ProjectionMatrix(glm::mat4(1.0)) {}
  ~Transform() {}
  const glm::mat4& GetModelMatrix() const { return m_ProjectionMatrix; }

  // Transformation function
  void Translate(const glm::vec3& translation) { m_ProjectionMatrix = GetTranslated(translation); }
  glm::mat4 GetTranslated(const glm::vec3& translation) { return glm::translate(m_ProjectionMatrix, translation); }

  void Scale(const glm::vec3 scale) { m_ProjectionMatrix = glm::scale(m_ProjectionMatrix, scale); }
  void Scale(float scale) { Scale(glm::vec3(scale)); }

  void Rotate(float angle, const glm::vec3& axis) { m_ProjectionMatrix = glm::rotate(m_ProjectionMatrix, angle, axis); }
protected:
  glm::mat4 m_ProjectionMatrix;
};

class Transform2D : public Transform
{
public:
  // Transformation function
  void Translate2D(const glm::vec2& translation) { Transform::Translate(glm::vec3(translation, 0.0f)); }
  glm::mat4 GetTranslated2D(const glm::vec2& translation) { return Transform::GetTranslated(glm::vec3(translation, 0.0f)); }

  void Scale2D(const glm::vec2 scale) { Transform::Scale(glm::vec3(scale, 0.0f)); }
  void Scale2D(float scale) { Scale2D(glm::vec2(scale)); }

  void Rotate2D(float angle) { Transform::Rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }
};