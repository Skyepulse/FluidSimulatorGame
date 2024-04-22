#include "VertexArray.h"

#include <glad/glad.h>
#include <memory>

GLenum ShaderDataTypeToBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:
	case ShaderDataType::Float2:
	case ShaderDataType::Float3:
	case ShaderDataType::Float4:
	case ShaderDataType::Mat3:
	case ShaderDataType::Mat4:
		return GL_FLOAT;
	case ShaderDataType::Int:
	case ShaderDataType::Int2:
	case ShaderDataType::Int3:
	case ShaderDataType::Int4:
		return GL_INT;
	case ShaderDataType::Bool:
		return GL_BOOL;
	}

	//CORE_ASSERT(false, "Unknown ShaderDataType !");
	return GL_NONE;
}

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);
}
void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	//CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();
	const auto& layout = vertexBuffer->GetLayout();
	int index = 0;
	for (const auto& element : layout)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.GetComponentCount(), ShaderDataTypeToBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
		index++;
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}