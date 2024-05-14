#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/Texture.h"

#include "../Scene/Shape.h"

#include "Transform.h"
#include "Camera.h"
#include "RendererInfos.h"

#include "../../Simulator/Solver.h"

#include "glad/glad.h"

/// @brief For all rendering actions on the current context (= window)
class Renderer
{
public:
  static void BeginScene(Camera& camera);
  static void EndScene();

  static void Draw(const Transform& transform, const RendererData& rendererData);

  static void DrawShape(std::shared_ptr<Shape> shape);
  static void DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<glm::vec2> positions);
  static void DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<Vec2f> positions);

  static void DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<glm::vec2> positions, std::vector<glm::vec2> rotations);
private:
  static glm::mat4 m_VPMatrix;
};