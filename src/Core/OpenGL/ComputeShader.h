#pragma once

#include "RenderTexture.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>

typedef unsigned int GLenum;

namespace Judas_Engine
{
	class ComputeShader
	{
	public:
		ComputeShader(const std::string& filepath);
		ComputeShader(const std::string& name, const std::string& src);

		const std::string GetName() { return m_Name; }

		void Dispatch(int xSize = 1, int ySize = 1, int zSize = 1) const;

	private:
		std::string ReadFile(const std::string& filepath);
		void Compile(const std::string& src);

	private:
		std::string m_Name;
		unsigned int m_RendererID;	

		std::shared_ptr<RenderTexture2D> m_RenderTexture;
		uint32_t m_TextureSlot;
	};
}