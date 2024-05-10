#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"

#include "../Rendering/Transform.h"
#include "../Rendering/RendererInfos.h"

#include <memory>

class Circle
{
public:
  Circle(const glm::vec3& color = glm::vec3(1.0));
  ~Circle();

  const RendererData& GetRendererData() const { return m_RendererData; }

  const glm::vec3& GetColor() const { return m_Color; }
  void SetColor(const glm::vec3& color) { m_Color = color;}
public:
  std::shared_ptr<Transform2D> m_Transform;
private:
  RendererData m_RendererData;
  glm::vec3 m_Color;
};