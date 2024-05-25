#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "cubicSpline.h"
#include <unordered_map>
#include <cmath>
#include <memory>
#include <vector>
#include "../Core/Log.h"


using namespace std;

struct ParticleManager {
	vector<Vec2f> pos; // position
	vector<Vec2f> vel; // velocity
	vector<Vec2f> acc; // acceleration
	vector<Real> press; // pressure
	vector<Real> density; // density
	vector<int> type; // type of particle
	vector<Real> alpha; // alpha
	vector<bool> isInGlass; // is particle in glass
};

struct Particle {
	Vec2f pos; // position
	Vec2f vel; // velocity
	Vec2f acc; // acceleration
	Real press; // pressure
	Real density; // density
	Real alpha; // alpha
	int type; // type of particle
	bool isInGlass; // is particle in glass
};

enum KernelType
{
	CUBIC_SPLINE = 0
};


class Solver
{
public:
	explicit Solver(
		const Real nu = 0.08, const Real h = 0.5, const Real density = 1e3,
		const Vec2f g = Vec2f(0, -9.8), const Real eta = 0.01, const Real gamma = 7e0,
		const Real dt = 0.0005, const KernelType kt = KernelType::CUBIC_SPLINE) :
		_nu(nu), _d0(density), _g(g), _eta(eta), _gamma(gamma), _dt(dt), _h(h)
	{
		_m0 = _d0 * _h * _h;
		_c  = std::fabs(_g.y) / _eta;
		_k = _d0 * _c * _c / _gamma;

		switch (kt)
		{
			case KernelType::CUBIC_SPLINE:
				_kernel = make_shared<CubicSpline>(h);
				break;
			default:
				_kernel = make_shared<CubicSpline>(h);
				break;
		}
	}

	void initSimulation(const Real resX, const Real resY);
	void init();
	void update();

	Real getH() const { return _h; }

	ParticleManager& getParticleManager() { return _pm; }

	int getParticlesInGlass() { return _particlesInGlass; }
	

private:
	inline tIndex idx1d(const int i, const int j) { return i + j * _resX;}
	void addParticle(const Vec2f& pos, const int type = 0, const Vec2f& vel = Vec2f(0e0	), const Vec2f& acc = Vec2f(0e0), const Real press = 0e0, const Real density = 0e0, const Real alpha = 0e0);
	Particle removeParticle(const tIndex index);

	void buildNeighbors();
	void computeDensity();
	void computeAlpha();

	void computeNPforces();
	void adaptDt();
	void predictVel(const Real dt);
	void correctDensityError(const Real dt);
	void updatePos(const Real dt);
	void correctDivergenceError(const Real dt);
	void computePressure();	

	void drawWalls(int resX, int resY);
	void drawStraightLineWall(const Vec2f& p1, int particleLength, int type = 1);
	void drawAngleLineWall(const Vec2f& p1, int particleLength, Real angle, int type = 1);

	void drawWinningGlass(int width, int height, Vec2f cornerPosition);

	shared_ptr<Kernel> _kernel;
	Real _nu, _d0, _m0, _k, _eta, _gamma, _dt;
	Real _resX, _resY;
	Real _h;
	Vec2f _g;
	Real _c;
	const Real DEFAULT_DT = 0.1f;

	Vec2f _glasscorner;
	Vec2f _glassSize;
	int _particlesInGlass = 0;

	tIndex _particleCount;
	tIndex _immovableParticleCount;

	ParticleManager _pm;
	vector<vector<tIndex>> _particlesInGrid;
	vector<vector<tIndex>> _neighbors;
};

#endif // !_SOLVER_H_
