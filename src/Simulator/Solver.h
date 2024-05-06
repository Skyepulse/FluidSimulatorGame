#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "cubicSpline.h"

using namespace std;
class Solver
{
public:
	explicit Solver(
		const Real nu = 0.08, const Real h = 0.5, const Real density = 1e3,
		const Vec2f g = Vec2f(0, -9.8), const Real eta = 0.01, const Real gamma = 7e0,
		const Real dt = 0.0005, const string kernelType = "cubicSpline") :
		_nu(nu), _d0(density), _g(g), _eta(eta), _gamma(gamma), _dt(dt)
	{
		_m0 = _d0 * _h * _h;
		_c  = std::fabs(_g.y) / _eta;
		_k = _d0 * _c * _c / _gamma;
		switch (kernelType)
		{
			case "cubicSpline":
				_kernel = CubicSpline(h);
				break;
			default:
				_kernel = CubicSpline(h);
				break;
		}
	}

	void initSimulation();
private:
	Kernel _kernel;
	Real _nu, _d0, _m0, _k, _eta, _gamma, _dt;
	Vec2f _g;
	Real _c;
};

#endif // !_SOLVER_H_
