#pragma once

#include <cstdint>

#include <glm/glm.hpp>

class Texture
{
public:
  virtual void Bind(uint32_t slot) const = 0;
  virtual void Unbind() const = 0;
protected:
  uint32_t m_RendererID;
  mutable uint32_t m_Slot;
};

class Texture2D : public Texture
{
public:
	Texture2D() {}
	Texture2D(int width, int height, int nrChannels);
	Texture2D(const char* texturePath);
	~Texture2D();

  virtual void Bind(uint32_t slot) const override;
  virtual void Unbind() const override;

  const glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height); }
  uint32_t GetWidth() const { return m_Width; }
  uint32_t GetHeight() const { return m_Height; }
  uint32_t GetChannelNumber() const { return m_NrChannels; }
private:
  static uint32_t GenerateTexture(const char* texturePath, int* width, int* height, int* nrChannels);
protected:
  int m_Width, m_Height, m_NrChannels;
};