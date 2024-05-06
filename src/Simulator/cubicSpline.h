#ifndef _CUBIC_SPLINE_H_
#define _CUBIC_SPLINE_H_

#include "kernel.h"

class CubicSpline : public Kernel
{
public:
	explicit CubicSpline(const Real h = static_cast<Real>(1.0)): _dim(2)
	{
		setSmoothingLength(h);
	}

	void setSmoothingLength(const Real h) override;
	Real getSmoothingLength() const override;
	Real getSupportRadius() const override;
	Real f(const Real l) const override;
	Real derivative_f(const Real l) const override;
	Real W(const Vec2f& r) const override;
	Vec2f gradW(const Vec2f& r) const override;
	Vec2f gradW(const Vec2f& r, const Real l) const override;

private:
	unsigned int _dim;
	Real _h, _sr, _c[3], _gc[3];
};
#endif // !_CUBIC_SPLINE_H_
