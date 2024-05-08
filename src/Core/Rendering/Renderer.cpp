#include "Renderer.h"

glm::mat4 Renderer::m_VPMatrix = glm::mat4(1.0);

void Renderer::BeginScene(Camera& camera)
{
  m_VPMatrix = camera.GetVPMatrix();
}

void Renderer::EndScene() {}

void Renderer::Draw(const Transform& transform, std::shared_ptr<Shader> shader, const VertexArray& va)
{
  va.Bind();
  shader->Bind();

  shader->SetMat4("u_VPMatrix", m_VPMatrix);
  shader->SetMat4("u_ModelMatrix", transform.GetModelMatrix());

  glDrawElements(GL_TRIANGLES, va.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Draw(const Transform &transform, std::shared_ptr<Shader> shader, const Texture& texture, const VertexArray& va)
{
  va.Bind();
  shader->Bind();

  shader->SetMat4("u_VPMatrix", m_VPMatrix);
  shader->SetMat4("u_ModelMatrix", transform.GetModelMatrix());

  texture.Bind();

  shader->SetInt("u_Texture", 0);

  glDrawElements(GL_TRIANGLES, va.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
  texture.Unbind();
}

void Renderer::DrawCircle(const Circle &circle)
{
  Draw(*circle.m_Transform, circle.GetShader(), circle.GetVertexArray());
}

void Renderer::DrawCircleDuplicate(std::vector<glm::vec2> positions, const Circle &circle)
{
  Transform2D transform = *circle.m_Transform;
  std::shared_ptr<Shader> shader = circle.GetShader();
  const VertexArray va = circle.GetVertexArray();

  for (auto position : positions)
  {
    transform.Translate2D(glm::vec2(position.x, position.y)); 
    Draw(transform, shader, va);
    // TEMPPP
    transform.Translate2D(glm::vec2(-position.x, -position.y)); 
  }
  
}
