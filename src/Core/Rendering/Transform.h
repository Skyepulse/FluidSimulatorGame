#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
  Transform();
  ~Transform();
  const glm::mat4& GetModelMatrix() const;

  // Transformation 
  ///@brief Translate the current transform
  void Translate(const glm::vec3& translation) { m_Position = glm::translate(m_Position, translation); }
  ///@brief Return a copy of the Transform translated
  Transform GetTranslated(const glm::vec3& translation) const {Transform transform(*this); transform.Translate(translation); return transform; }

  void SetPosition(const glm::vec3& position) { m_Position = glm::translate(glm::mat4(1.0), position); }


  void Scale(const glm::vec3& scale) { m_Scale = glm::scale(m_Scale, scale); }
  void Scale(float scale) { Scale(glm::vec3(scale)); }
  Transform GetScaled(const glm::vec3& scale) const {Transform transform(*this); transform.Scale(scale); return transform; }
  Transform GetScaled(float scale) const {Transform transform(*this); transform.Scale(scale); return transform; }

  void Rotate(float angle, const glm::vec3& axis) { m_Rotation = glm::rotate(m_Rotation, angle, axis); }
  Transform GetRotated(float angle, const glm::vec3& axis) const {Transform transform(*this); transform.Rotate(angle, axis); return transform; }
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
  Transform2D GetTranslated2D(const glm::vec2& translation) const {Transform2D transform(*this); transform.Translate2D(translation); return transform; }

  void SetPosition2D(const glm::vec2& position) { SetPosition(glm::vec3(position, 0.0)); }

  void Scale2D(const glm::vec2 scale) { Scale(glm::vec3(scale, 0.0f)); }
  void Scale2D(float scale) { Scale2D(glm::vec2(scale)); }
  Transform2D GetScaled(const glm::vec2& scale) const {Transform2D transform(*this); transform.Scale2D(scale); return transform; }
  Transform2D GetScaled(float scale) const {Transform2D transform(*this); transform.Scale2D(scale); return transform; }

  void SetSize(const glm::vec2 size);

  void Rotate2D(float angle) { Rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }
  Transform2D GetRotated(float angle, const glm::vec3& axis) const {Transform2D transform(*this); transform.Rotate2D(angle); return transform; }

  void SetRotation2D(float angle) { SetRotation(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }
  void SetDirection(const glm::vec2& direction); // TO DO : Maybe keep a direction vector ? Must be coherent with matrices !
  glm::vec2 GetPosition();
};