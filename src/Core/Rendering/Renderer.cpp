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