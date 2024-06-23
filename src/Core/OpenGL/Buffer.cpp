#include "Buffer.h"

// VertexBuffer Class

VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// IndexBuffer Class

IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
	: m_Count(count)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// SSBO Class

DataBufferObject::DataBufferObject(const void* data, size_t size) : m_Size(size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
	UpdateData(data);
	Unbind();
}

void DataBufferObject::Bind(unsigned int index) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_RendererID);
}

void DataBufferObject::Unbind() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void DataBufferObject::UpdateData(const void* data, uint32_t size) const
{
	if(size == 0)
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_Size, data, GL_DYNAMIC_READ);
	else
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_READ);

}