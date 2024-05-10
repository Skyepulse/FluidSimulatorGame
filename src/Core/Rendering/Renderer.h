#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/Texture.h"

// TEMP TO REMOVE
#include "../Scene/Circle.h"

#include "Transform.h"
#include "Camera.h"
#include "RendererInfos.h"

#include "glad/glad.h"

/// @brief For all rendering actions on the current context (= window)
class Renderer
{
public:
  static void BeginScene(Camera& camera);
  static void EndScene();

  static void Draw(const Transform& transform, const RendererData& rendererData);
  static void Draw(const Transform& transform, const RendererData& rendererData, const Texture& texture);

  static void DrawCircle(const Circle& circle);
  static void DrawCircleDuplicate(std::vector<glm::vec2> positions, const Circle& circle);
private:
  static glm::mat4 m_VPMatrix;
};