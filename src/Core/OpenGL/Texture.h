#pragma once

#include <cstdint>

class Texture
{
public:
  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
protected:
  uint32_t m_RendererID;
};

class Texture2D : public Texture
{
public:
  Texture2D(const char* texturePath);
  ~Texture2D();

  virtual void Bind() const override;
  virtual void Unbind() const override;
private:
  static uint32_t GenerateTexture(const char* texturePath, int* width, int* height, int* nrChannels);
private:
  int m_Width, m_Height, m_NrChannels;
};