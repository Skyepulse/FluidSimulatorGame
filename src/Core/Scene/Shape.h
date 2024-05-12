#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"

#include "../Rendering/Transform.h"
#include "../Rendering/RendererInfos.h"

#include <glm/glm.hpp>

struct ShapeInfos
{
  std::string vertexShaderFilepath;
  std::string fragmentShaderFilepath;

  mutable std::vector<float> vertices;
  mutable std::vector<uint32_t> indices;
  BufferLayout layout;
  GLenum renderMode;
};

class Shape
{
public:
  ~Shape();

  const RendererData& GetRendererData() const { return m_RendererData; }

  const glm::vec3& GetColor() const { return m_Color; }
  void SetColor(const glm::vec3& color) { m_Color = color;}
protected:
  Shape();
  void SetRendererData(const ShapeInfos& shapeInfos);

public:
  std::shared_ptr<Transform2D> Transform;
protected:
  RendererData m_RendererData;
  glm::vec3 m_Color;
};