#pragma once

#include <memory>

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"

struct RendererData
{
  mutable std::shared_ptr<Shader> shader;
  VertexArray vertexArray;
  GLenum renderMode;
};
