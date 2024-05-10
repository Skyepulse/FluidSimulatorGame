#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
  Transform();
  ~Transform();
  const glm::mat4& GetModelMatrix() const;

  // Transformation function
  void Translate(const glm::vec3& translation) { m_Position = glm::translate(m_Position, translation); }

  void Scale(const glm::vec3 scale) { m_Scale = glm::scale(m_Scale, scale); }
  void Scale(float scale) { Scale(glm::vec3(scale)); }

  void Rotate(float angle, const glm::vec3& axis) { m_Rotation = glm::rotate(m_Rotation, angle, axis); }
  void SetRotation(float angle, const glm::vec3& axis) { m_Rotation = glm::rotate(glm::mat4(1.0), angle, axis); }
private:
  void ComputeModelMatrix() const { m_ModelMatrix = m_Position * m_Rotation * m_Scale; }
protected:
  mutable glm::mat4 m_ModelMatrix;
  glm::mat4 m_Position, m_Rotation, m_Scale;
};

class Transform2D : public Transform
{
public:
  // Transformation function
  void Translate2D(const glm::vec2& translation) { Translate(glm::vec3(translation, 0.0f)); }

  void Scale2D(const glm::vec2 scale) { Scale(glm::vec3(scale, 0.0f)); }
  void Scale2D(float scale) { Scale2D(glm::vec2(scale)); }

  void Rotate2D(float angle) { Rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }
  void SetRotation2D(float angle) { SetRotation(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }

  void SetDirection(const glm::vec2& direction); // TO DO : Maybe keep a direction vector ? Must be coherent with matrices !

};