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

void Renderer::DrawPoint(const Transform &transform, std::shared_ptr<Shader> shader, const VertexArray& va)
{
  va.Bind();
  shader->Bind();

  shader->SetMat4("u_VPMatrix", m_VPMatrix);
  shader->SetMat4("u_ModelMatrix", transform.GetModelMatrix());

  glEnable(GL_SMOOTH_POINT_SIZE_RANGE);
  glPointSize(50);
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDrawElements(GL_POINTS, va.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

  shader->Unbind();
  va.Unbind();
}
