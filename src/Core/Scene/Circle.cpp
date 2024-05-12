#include "Circle.h"

Circle::Circle()
	: Shape()
{
	m_ShapeInfos.vertexShaderFilepath = std::string("src/shader.vertex"), 
  m_ShapeInfos.fragmentShaderFilepath = std::string("src/circleShader.fragment"),
    
    // Vertex Buffer
  m_ShapeInfos.vertices =  {
		-0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
			0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	};

	 // Index Buffer
  m_ShapeInfos.indices = 
	{
		0, 1, 2,
		2, 3, 0
	};

	// Buffer Layout
  m_ShapeInfos.layout = BufferLayout({
		{ShaderDataType::Float3, "v_Position"},
		{ShaderDataType::Float3, "v_Color"},
		{ShaderDataType::Float2, "v_TexCoords"},
	});

  // Render Mode
	m_ShapeInfos.renderMode = GL_TRIANGLES;

	SetRendererData(m_ShapeInfos);
}

Circle::~Circle()
{
}
