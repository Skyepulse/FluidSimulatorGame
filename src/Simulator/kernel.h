#ifndef _KERNEL_INTERFACE_
#define _KERNEL_INTERFACE_

#include "vector.h"

class Kernel {
public:
	virtual void setSmoothingLength(const Real h) = 0;
	virtual Real getSmoothingLength() const = 0;
	virtual Real f(const Real l) const = 0;
	virtual Real derivative_f(const Real l) const = 0;
	virtual Real W(const Vec2f& r) const = 0;
	virtual Vec2f gradW(const Vec2f& r) const = 0;
	virtual Vec2f gradW(const Vec2f& r, const Real l) const = 0;
	virtual Vec2f laplW(const Vec2f& r) const = 0;
};

#endif 
