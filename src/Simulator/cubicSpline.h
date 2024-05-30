#ifndef _CUBIC_SPLINE_H_
#define _CUBIC_SPLINE_H_

#define LOOKUP_SIZE 1000

#include "kernel.h"

class CubicSpline : public Kernel
{
public:
	explicit CubicSpline(const Real h = static_cast<Real>(1.0)): _dim(2)
	{
		setSmoothingLength(h);
		generateLookup();
	}

	void setSmoothingLength(const Real h) override;
	Real f(const Real l) const override;
	Real derivative_f(const Real l) const override;
	Real W(const Vec2f& r) const override;
	Vec2f gradW(const Vec2f& r) const override;
	Vec2f gradW(const Vec2f& r, const Real l) const override;
	Real laplW(const Vec2f& r) const override;
	void generateLookup();

	Real getSupportRad() const override {return _sr;}

	//Get lookup table
	Real* getLookupF() override {return lookup_f;}
	Real* getLookupDF() override {return lookup_df;}

private:
	unsigned int _dim;
	Real _c[3], _gc[3], _lc[3];
	Real _sr;
	Real lookup_f[LOOKUP_SIZE], lookup_df[LOOKUP_SIZE];
};
#endif // !_CUBIC_SPLINE_H_
