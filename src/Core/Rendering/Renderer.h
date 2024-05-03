#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "Transform.h"
#include "Camera.h"

#include "glad/glad.h"

/// @brief For all rendering actions on the current context (= window)
class Renderer
{
public:
  static void BeginScene(Camera& camera)
  {
    m_VPMatrix = camera.GetVPMatrix();
  }

  static void EndScene() { }

  static void Draw(const Transform& transform, std::shared_ptr<Shader> shader, const VertexArray& va)
  {
    va.Bind();
		shader->Bind();

    shader->SetMat4("u_VPMatrix", m_VPMatrix);
		shader->SetMat4("u_ModelMatrix", transform.GetModelMatrix());

  	glDrawElements(GL_TRIANGLES, va.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
  }
private:
  static glm::mat4 m_VPMatrix;
};

glm::mat4 Renderer::m_VPMatrix;