#include "Shape.h"

Shape::Shape()
{  
  // TRANSFORM
  Transform = std::make_shared<Transform2D>();
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

