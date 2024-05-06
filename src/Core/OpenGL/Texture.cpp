#include "Texture.h"

#include "../Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../dep/stb_image.h"

#include <glad/glad.h>

Texture2D::Texture2D(const char* texturePath)
{
  m_RendererID = GenerateTexture(texturePath, &m_Width, &m_Height, &m_NrChannels);
}

Texture2D::~Texture2D()
{

}

void Texture2D::Bind() const
{
  // TEMP
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture2D::Unbind() const
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t Texture2D::GenerateTexture(const char* texturePath, int* width, int* height, int* nrChannels)
{
  unsigned int textureID;
	glGenTextures(1, &textureID);

	const char* path = "src/texture.jpeg";
	unsigned char *data = stbi_load(path, width, height, nrChannels, 0);
	if (data)
	{
			GLenum format;
			if (*nrChannels == 1)
					format = GL_RED;
			else if (*nrChannels == 3)
					format = GL_RGB;
			else if (*nrChannels == 4)
					format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, *width, *height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		CORE_ERROR("Texture failed to load at path: {}.\n stb error : {}", path, stbi_failure_reason());
	}

  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, textureID);

  return textureID;
}
