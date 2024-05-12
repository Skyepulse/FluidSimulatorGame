#include "Line.h"

Line::Line()
	: Shape()
{
	m_ShapeInfos.vertexShaderFilepath = std::string("src/shader.vertex"), 
  m_ShapeInfos.fragmentShaderFilepath = std::string("src/shader.fragment"),
    
    // Vertex Buffer
  m_ShapeInfos.vertices =  {
		0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
	};

	 // Index Buffer
  m_ShapeInfos.indices = {
		0, 1,
	};

	// Buffer Layout
  m_ShapeInfos.layout = BufferLayout({
		{ShaderDataType::Float3, "v_Position"},
		{ShaderDataType::Float3, "v_Color"},
		{ShaderDataType::Float2, "v_TexCoords"},
	});

  // Render Mode
	m_ShapeInfos.renderMode = GL_LINES;

	SetRendererData(m_ShapeInfos);
}

Line::~Line()
{
}
