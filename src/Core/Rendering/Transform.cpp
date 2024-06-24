#include "Transform.h"

#include "../Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

Transform::Transform() 
  : m_ModelMatrix(glm::mat4(1.0)), m_Position(glm::mat4(1.0)), m_Rotation((glm::mat4(1.0))), m_Scale(glm::mat4(1.0))
{

}

Transform::~Transform()
{
}

const glm::mat4 &Transform::GetModelMatrix() const
{
  ComputeModelMatrix(); 
  return m_ModelMatrix;
}

void Transform2D::SetSize(const glm::vec2 size)
{
	m_Scale = glm::mat4(1.0);
	Scale2D(size);
}

void Transform2D::SetDirection(const glm::vec2& direction)
{
  glm::vec2 normalizedDirection = glm::normalize(direction);
  double angle = glm::orientedAngle(glm::vec2(0.0f, 1.0f), direction);
  SetRotation2D(angle);
}

glm::vec2 Transform2D::GetPosition() {
    return m_Position * glm::vec4(0, 0, 0, 1);
}
