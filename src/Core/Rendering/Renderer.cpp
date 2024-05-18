#include "Renderer.h"

glm::mat4 Renderer::m_VPMatrix = glm::mat4(1.0);

void Renderer::BeginScene(Camera& camera)
{
  m_VPMatrix = camera.GetVPMatrix();
}

void Renderer::EndScene() {}

void Renderer::Draw(const Transform& transform, const RendererData& rendererData)
{
  rendererData.vertexArray.Bind();
  rendererData.shader->Bind();

  rendererData.shader->SetMat4("u_VPMatrix", m_VPMatrix);
  rendererData.shader->SetMat4("u_ModelMatrix", transform.GetModelMatrix());

  for(auto& renderProperty : rendererData.renderProperties)
  {
    renderProperty->SetProperties(rendererData.shader);
  }

  glDrawElements(rendererData.renderMode, rendererData.vertexArray.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawShape(std::shared_ptr<Shape> shape)
{
  Draw(*shape->Transform, shape->GetRendererData());
}

void Renderer::DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<glm::vec2> positions)
{
  Transform2D transform = *shape->Transform;
  const RendererData rendererData = shape->GetRendererData();

  for (auto position : positions)
  {
    Transform2D dupTransform = transform.GetTranslated2D(position);
    Draw(dupTransform, rendererData);
  }
}

void Renderer::DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<Vec2f> positions)
{
  Transform2D transform = *shape->Transform;
  const RendererData rendererData = shape->GetRendererData();

  for (auto position : positions)
  {
    Transform2D dupTransform = transform.GetTranslated2D(glm::vec2(position.x, position.y));
    Draw(dupTransform, rendererData);
  }
}

void Renderer::DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<glm::vec2> positions, std::vector<glm::vec2> directions)
{
  Transform2D transform = *shape->Transform;
  const RendererData rendererData = shape->GetRendererData();

  for (size_t i = 0; i < positions.size(); i++)
  {
    Transform2D dupTransform = transform.GetTranslated2D(positions[i]);
    dupTransform.SetDirection(directions[i]);
    Draw(dupTransform, rendererData);
  }
}

void Renderer::DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<Vec2f> positions, std::vector<Vec2f> directions)
{
  Transform2D transform = *shape->Transform;
  const RendererData rendererData = shape->GetRendererData();

  for (size_t i = 0; i < positions.size(); i++)
  {
    Transform2D dupTransform = transform.GetTranslated2D(glm::vec2(positions[i].x, positions[i].y));
    dupTransform.SetDirection(glm::vec2(directions[i].x, directions[i].y));
    Draw(dupTransform, rendererData);
  }
}

void Renderer::DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<glm::vec2> positions, std::vector<glm::vec2> directions, std::vector<glm::vec2> scales)
{
  Transform2D transform = *shape->Transform;
  const RendererData rendererData = shape->GetRendererData();

  for (size_t i = 0; i < positions.size(); i++)
  {
    Transform2D dupTransform = transform.GetTranslated2D(positions[i]);
    dupTransform.SetDirection(directions[i]);
    dupTransform.Scale2D(scales[i]);
    Draw(dupTransform, rendererData);
  }
}

void Renderer::DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<glm::vec2> positions, std::vector<float> value, const glm::vec3 &color)
{
  Transform2D transform = *shape->Transform;
  const RendererData rendererData = shape->GetRendererData();

  for (size_t i = 0; i < positions.size(); i++)
  {
    Transform2D dupTransform = transform.GetTranslated2D(positions[i]);
    shape->SetColor(value[i] * color);
    Draw(dupTransform, rendererData);
  }
}

void Renderer::DrawShapeDuplicate(std::shared_ptr<Shape> shape, std::vector<Vec2f> positions, std::vector<float> value, const glm::vec3 &color)
{
  Transform2D transform = *shape->Transform;
  const RendererData rendererData = shape->GetRendererData();

  for (size_t i = 0; i < positions.size(); i++)
  {
    Transform2D dupTransform = transform.GetTranslated2D(glm::vec2(positions[i].x, positions[i].y));
    shape->SetColor(value[i] * color);
    Draw(dupTransform, rendererData);
  }
}
