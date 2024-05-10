#include "Circle.h"

#include <glad/glad.h>

Circle::Circle(const glm::vec3& color) : m_Color(color)
{
  // SHADER 
  m_RendererData.shader = std::make_shared<Shader>("src/shader.vertex", "src/pointShader.fragment");

  // VERTEX ARRAY DATA
	std::vector<float> vertices = {
		-0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	};
	std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
	BufferLayout layout({
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float3, "a_Color"},
		{ShaderDataType::Float2, "a_TexCoords"},
	});
	vb->SetLayout(layout);

	std::vector<uint32_t> index = {
		0, 1, 2,
		2, 3, 0
	};

	std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(index.data(), index.size());

	m_RendererData.vertexArray.AddVertexBuffer(vb);
	m_RendererData.vertexArray.SetIndexBuffer(ib);
	m_RendererData.renderMode = GL_TRIANGLES;

  // TRANSFORM
  m_Transform = std::make_shared<Transform2D>();
}

Circle::~Circle()
{
}
