#pragma once

#include "Texture.h"

#include <cstdint>

class RenderTexture2D : public Texture2D
{
public:
	RenderTexture2D() : Texture2D() {}
	RenderTexture2D(uint32_t width, uint32_t height);
	~RenderTexture2D();

	void BindImage(uint32_t slot);
	void UnbindImage() const;
 
private:
	mutable uint32_t m_ImageSlot;
};
