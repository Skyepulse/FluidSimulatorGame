#pragma once

#include <string>
#include <vector>
#include <memory>

enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Mat3, Mat4, Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type) 
{
	switch (type)
	{
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;
		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Bool:		return 1;
	}

	//CORE_ASSERT(false, "Unknown ShaderDataType !");
	return 0;
}

struct BufferElement 
{
	std::string Name;
	ShaderDataType Type;
	uint32_t Offset;
	uint32_t Size;
	bool Normalized;

	BufferElement() {}

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
	{ }

	uint32_t GetComponentCount() const
	{
		switch (Type)
		{
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Mat3:		return 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
		}

		//CORE_ASSERT(false, "Unknown ShaderDataType !");
		return 0;
	}
};

class BufferLayout
{
public: 
	BufferLayout() {}

	BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_Elements(elements)
	{
		CalculateOffsetAndStride();
	}

	inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
	inline uint32_t GetStride() const { return m_Stride; }

	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }	
	std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
private:
	void CalculateOffsetAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}
private:
	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride;
};

class VertexBuffer
{
public:
	VertexBuffer(float* vertices, uint32_t size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
	const BufferLayout& GetLayout() const { return m_Layout; }
private:
	uint32_t m_RendererID;
	BufferLayout m_Layout;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t size);
	virtual ~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	uint32_t GetCount() const { return m_Count; }
private:
	uint32_t m_RendererID;
	uint32_t m_Count;
};

template <typename T>
class BufferObject
{
public:
	BufferObject(const T* data, size_t size);
	~BufferObject() {}

	void Bind(unsigned int index = 0) const;
	void Unbind() const;

	void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
	const BufferLayout& GetLayout() const { return m_Layout; }

private:
	unsigned int m_RendererID;
	unsigned int m_Size;
	BufferLayout m_Layout;
};

class DataBufferObject
{
public:
	DataBufferObject() {}
	DataBufferObject(const void* data, size_t size);
	~DataBufferObject() {}

	void Bind(unsigned int index = 0) const;
	void Unbind() const;

	void UpdateData(const void* data, uint32_t size = 0) const;

private:
	unsigned int m_RendererID; 
	unsigned int m_Size;
};

// Def BufferObject

template<typename T>
inline BufferObject<T>::BufferObject(const T* data, size_t size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_READ);
}

template<typename T>
inline void BufferObject<T>::Bind(unsigned int index) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

template<typename T>
inline void BufferObject<T>::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
