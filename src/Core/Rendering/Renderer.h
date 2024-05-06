#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/Texture.h"

#include "Transform.h"
#include "Camera.h"

#include "glad/glad.h"

/// @brief For all rendering actions on the current context (= window)
class Renderer
{
public:
  static void BeginScene(Camera& camera);
  static void EndScene();

  static void Draw(const Transform& transform, std::shared_ptr<Shader> shader, const VertexArray& va);
  static void Draw(const Transform& transform, std::shared_ptr<Shader> shader, const Texture& texture, const VertexArray& va);
private:
  static glm::mat4 m_VPMatrix;
};