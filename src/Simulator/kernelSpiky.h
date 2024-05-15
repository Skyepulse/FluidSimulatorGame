#ifndef _KERNEL_SPIKY_H_
#define _KERNEL_SPIKY_H_

#include "kernel.h"

class KernelSpiky : public Kernel
{
public:
	explicit KernelSpiky(const Real h = static_cast<Real>(1.0))
	{
		setSmoothingLength(h);
	}

    void setSmoothingLength(const Real h) override;
	Real W(const Vec2f& r) const override;
	Vec2f gradW(const Vec2f& r) const override;
	Real laplW(const Vec2f& r) const override;

private:
    Real _h2, _h3, _1h2, _1h3;
    Real _c, _gc, _lc;
};
#endif // !_KERNEL_SPIKY_H_
