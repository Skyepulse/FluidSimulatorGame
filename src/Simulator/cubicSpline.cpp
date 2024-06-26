#include "cubicSpline.h"


void CubicSpline::setSmoothingLength(const Real h)
{
	_sr = h*2e0;
	_h = h;
	const Real h2 =square(_h), h3 = h2 * _h;
	_c[0] = 2e0 / (3e0 * _h);
	_c[1] = 10e0 / (7e0 * M_PI * h2);
	_c[2] = 1e0 / (M_PI * h3);
	_gc[0] = _c[0] / _h;
	_gc[1] = _c[1] / _h;
	_gc[2] = _c[2] / _h;
	_lc[0] = _gc[0] / _h;
	_lc[1] = _gc[1] / _h;
	_lc[2] = _gc[2] / _h;
}

Real CubicSpline::f(const Real l) const
{
	const Real q = l / _h;
	if(q <= 1e0)
		return _c[_dim -1] * (1e0 - 1.5 * square(q) + 0.75 * cube(q));
	else if(q < 2e0)
		return _c[_dim -1] * 0.25 * cube(2e0 - q);
	return 0e0;
}	

Real CubicSpline::derivative_f(const Real l) const
{
	const Real q = l / _h;
	if(q < 1e0)
		return _gc[_dim -1] * (-3e0 * q + 2.25 * square(q));
	else if(q < 2e0)
		return _gc[_dim -1] * (-0.75 * square(2e0 - q));
	return 0e0;
}

Real CubicSpline::W(const Vec2f& r) const
{
	const Real l = r.length();
	if (l >= _sr) return 0.0;
	
	return lookup_f[(int) (l / _sr * LOOKUP_SIZE)];

	//return f(l);
}

Vec2f CubicSpline::gradW(const Vec2f& r) const
{
	const Real l = r.length();
	return gradW(r, l);
}

Vec2f CubicSpline::gradW(const Vec2f& r, const Real l) const
{
	if (l == 0){
		return Vec2f(0.0f);
	}
	if (l >= _sr) return Vec2f(0.0);
	
	return lookup_df[(int) (l / _sr * LOOKUP_SIZE)] * r / l;

	//return derivative_f(l) * r / l;
}

Real CubicSpline::laplW(const Vec2f& r) const {

	const Real l = r.length();
	const Real q = l/_h;
	
	Vec2f val;

	if (q < 1e0)
		val =  _lc[_dim -1] * (9.0/(4.0*_h*_h*l) * (r*r + l*l) - 3.0);
	else if (q < 2e0)
		val = _lc[_dim -1] * (-3*Vec2f(r.y*r.y, r.y*r.y)/(l*l*l) -3.0/(4.0*_h)*(r*r + l*l) + 3.0);
	else
		val = Vec2f(0);

	return val.x + val.y;
}

void CubicSpline::generateLookup(){
	for (int i=0; i<LOOKUP_SIZE; i++){
		lookup_f[i] = f(2.0*_h/LOOKUP_SIZE * i);
		lookup_df[i] = derivative_f(2.0*_h/LOOKUP_SIZE * i);
	}
}