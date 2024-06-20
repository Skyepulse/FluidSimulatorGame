#include "Shape.h"

void Shape::SetTexture(const Texture* texture)
{
	std::shared_ptr<RenderProperty> textureProperty;
	textureProperty = std::make_shared<TextureRenderProperty>(texture);
	m_RendererData.renderProperties.push_back(textureProperty);
}

Shape::Shape() : m_Color(glm::vec3(1.0))
{  
  // TRANSFORM
  Transform = std::make_shared<Transform2D>();

  // TODO : Enable add only desired render properties
  // Color property
  std::shared_ptr<RenderProperty> colorProperty;
  colorProperty = std::make_shared<ColorRenderProperty>(&m_Color);
  m_RendererData.renderProperties.push_back(colorProperty);
}

void Shape::SetRendererData(const ShapeInfos& shapeInfos)
{
  // SHADER 
  m_RendererData.shader = std::make_shared<Shader>(shapeInfos.vertexShaderFilepath, shapeInfos.fragmentShaderFilepath);

	std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(shapeInfos.vertices.data(), shapeInfos.vertices.size() * sizeof(float));
	vb->SetLayout(shapeInfos.layout);

	std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(shapeInfos.indices.data(), shapeInfos.indices.size());

	m_RendererData.vertexArray.AddVertexBuffer(vb);
	m_RendererData.vertexArray.SetIndexBuffer(ib);

  m_RendererData.renderMode = shapeInfos.renderMode;
}

Shape::~Shape()
{
}

