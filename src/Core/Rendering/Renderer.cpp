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

void Renderer::Draw(const Transform& transform, const RendererData& rendererData, const Texture& texture)
{
  rendererData.vertexArray.Bind();
  rendererData.shader->Bind();

  rendererData.shader->SetMat4("u_VPMatrix", m_VPMatrix);
  rendererData.shader->SetMat4("u_ModelMatrix", transform.GetModelMatrix());

  texture.Bind();

  rendererData.shader->SetInt("u_Texture", 0);

  glDrawElements(rendererData.renderMode, rendererData.vertexArray.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
  texture.Unbind();
}

void Renderer::DrawCircle(const Circle &circle)
{
  Draw(*circle.m_Transform, circle.GetRendererData());
}

void Renderer::DrawCircleDuplicate(std::vector<glm::vec2> positions, const Circle &circle)
{
  Transform2D transform = *circle.m_Transform;
  const RendererData rendererData = circle.GetRendererData();

  for (auto position : positions)
  {
    transform.Translate2D(glm::vec2(position.x, position.y)); 
    Draw(transform, rendererData);
    // TEMPPP
    transform.Translate2D(glm::vec2(-position.x, -position.y)); 
  }
  
}
