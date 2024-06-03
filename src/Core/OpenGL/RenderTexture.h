#pragma once

#include <cstdint>

namespace Judas_Engine
{
	class RenderTexture2D
	{
	public:
		RenderTexture2D(uint32_t width, uint32_t height);
		~RenderTexture2D();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		void Bind(uint32_t slot);
		void Unbind() const;

		void BindImage(uint32_t slot);
		void UnbindImage() const;
 
	private:
		unsigned int m_Slot, m_Unit;
		unsigned int m_Width, m_Height;
		unsigned int m_RenderID;
	};
}
