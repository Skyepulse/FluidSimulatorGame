#pragma once

#include "Texture.h"

#include <cstdint>

class RenderTexture2D : public Texture2D
{
public:
	RenderTexture2D() : Texture2D() {}
	RenderTexture2D(uint32_t width, uint32_t height);
	~RenderTexture2D();

	virtual void Bind(uint32_t slot) const override;
	virtual void Unbind() const override;
 
private:
	mutable uint32_t m_ImageSlot;
};
