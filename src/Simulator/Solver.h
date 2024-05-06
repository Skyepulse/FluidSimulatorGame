#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "cubicSpline.h"
#include <unordered_map>
using namespace std;

struct ParticleManager {
	vector<Vec2f> pos; // position
	vector<Vec2f> vel; // velocity
	vector<Vec2f> acc; // acceleration
	vector<Real> press; // pressure
	vector<Real> density; // density
};

struct Particle {
	Vec2f pos; // position
	Vec2f vel; // velocity
	Vec2f acc; // acceleration
	Real press; // pressure
	Real density; // density
};

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

	void initSimulation(const Real resX, const Real resY);
	void update(const Real dt);

	ParticleManager& getParticleManager() { return _pm; }
	

private:
	inline tIndex idx1d(const int i, const int j) { return i + j * resX();}
	void addParticle(const Vec2f& pos, const Vec2f& vel = 0e0, const Vec2f& acc = 0e0, const Real press = 0e0, const Real density = _d0);
	Particle removeParticle(const tIndex index);

	Kernel _kernel;
	Real _nu, _d0, _m0, _k, _eta, _gamma, _dt;
	Vec2f _g;
	Real _c;

	ParticleManager _pm;
	vector<vector<tIndex>> _particlesInGrid;
	vector<vector<tIndex>> _neighbors;
};

#endif // !_SOLVER_H_
