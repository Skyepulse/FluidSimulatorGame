#ifndef _KERNEL_INTERFACE_
#define _KERNEL_INTERFACE_

#include "vector.h"

#ifndef M_PI
#define M_PI 3.141592
#endif

class Kernel {
public:
	virtual void setSmoothingLength(const Real h){_h = h;};
	Real getSmoothingLength(){return _h;}
	virtual Real f(const Real l) const = 0;
	virtual Real derivative_f(const Real l) const = 0;
	virtual Real W(const Vec2f& r) const = 0;
	virtual Vec2f gradW(const Vec2f& r) const = 0;
	virtual Vec2f gradW(const Vec2f& r, const Real l) const = 0;
	virtual Real laplW(const Vec2f& r) const = 0;
	virtual Real getSupportRad() const = 0;

	virtual Real* getLookupF() = 0;
	virtual Real* getLookupDF() = 0;

protected:
	Real _h;
};

#endif 
