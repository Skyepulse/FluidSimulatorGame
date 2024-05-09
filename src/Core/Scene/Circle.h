#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"

#include "../Rendering/Transform.h"

#include <memory>

class Circle
{
public:
  Circle();
  ~Circle();

  const VertexArray& GetVertexArray() const { return m_VertexArray; }
  std::shared_ptr<Shader> GetShader() const { return m_Shader; }
public:
  std::shared_ptr<Transform2D> m_Transform;
private:
  std::shared_ptr<Shader> m_Shader;
  VertexArray m_VertexArray;
};