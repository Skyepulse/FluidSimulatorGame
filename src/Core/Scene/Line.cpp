#include "Line.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

Line::Line(const glm::vec3& color) : m_Color(color)
{
  // SHADER 
  m_RendererData.shader = std::make_shared<Shader>("src/shader.vertex", "src/shader.fragment");

  // VERTEX ARRAY DATA
	std::vector<float> vertices = {
		0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	};
	std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
	BufferLayout layout({
		{ShaderDataType::Float3, "v_Position"},
		{ShaderDataType::Float3, "v_Color"},
		{ShaderDataType::Float2, "v_TexCoords"},
	});
	vb->SetLayout(layout);

	std::vector<uint32_t> index = {
		0, 1,
	};

	std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(index.data(), index.size());

	m_RendererData.vertexArray.AddVertexBuffer(vb);
	m_RendererData.vertexArray.SetIndexBuffer(ib);

  // RENDER MODE !
	m_RendererData.renderMode = GL_LINES;

  // TRANSFORM
  Transform = std::make_shared<Transform2D>();

	// Color Render property
	std::shared_ptr<ColorRenderProperty> colorProperties = std::make_shared<ColorRenderProperty>(&m_Color);

	m_RendererData.renderProperties.push_back(colorProperties);
}

Line::~Line()
{
}
