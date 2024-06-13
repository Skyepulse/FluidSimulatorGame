#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "cubicSpline.h"
#include <unordered_map>
#include <cmath>
#include <memory>
#include <vector>
#include "../Core/Log.h"
#include <glad/glad.h>


using namespace std;

struct Particle {
	Vec2f pos; // position
	Vec2f vel; // velocity
	Vec2f acc; // acceleration
	Real density; // density
	Real alpha; // alpha
	int type; // type of particle
	bool isInGlass; // is particle in glass
	bool needUpdate=true;
};


struct ParticleGroup {
	tIndex startIdx;
	tIndex endIdx;
	vector<Vec2f> initPos;

	ParticleGroup() : startIdx(0), endIdx(0) {}

    ParticleGroup(tIndex start, tIndex end, const std::vector<Vec2f>& positions)
        : startIdx(start), endIdx(end), initPos(positions) {}
};

enum KernelType
{
	CUBIC_SPLINE = 0
};

enum ViscosityType
{
	FLUID = 0,
	VISCOUS = 1
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
		_c = std::fabs(_g.y) / _eta;
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

		initOpenGL();
	}



	void initSimulation(const Real resX, const Real resY);
	void init();
	Real update();

	//OpenGL compute shader methods
	void initOpenGL();
	void cleanupOpenGL();
	void predictVelCS(const Real dt);
	void computeDensityAlphaCS();

	Real getH() const { return _h; }

	vector<Particle>& getParticleManager() { return _particleData; }

	int getParticlesInGlass() { return _particlesInGlass; }
	int getWinningGlass() { return _winningGlass; }
	int getParticleCount() { return _particleCount; }
	int getImmovableParticleCount() { return _immovableParticleCount; }
	int getImmovableGlassParticleCount() { return _immovableGlassParticleCount; }
	Vec2f getSpawnPosition() { return _spawnPosition; }
	void spawnParticle(Vec2f position);

	void drawWalls(int resX, int resY);
	void drawStraightLineWall(const Vec2f& p1, int particleLength, int type = 1, bool save=true);
	void drawAngleLineWall(const Vec2f& p1, int particleLength, Real angle, int type = 1, bool save=true);
	void drawAngleRectangleWall(const Vec2f& p1, int width, int height, Real angle, int type = 1, bool save=true);

	void drawWinningGlass(int width, int height, Vec2f cornerPosition);
	void setSpawnPosition(Vec2f position);

	void moveGlassLeft(bool move) { _moveGlassLeft = move; }
	void moveGlassRight(bool move) { _moveGlassRight = move; }
	void moveGlassUp(bool move) { _moveGlassUp = move; }
	void moveGlassDown(bool move) { _moveGlassDown = move; }

	Vec2f getGlassPosition();
	void setGlassSpeed(Real speedX, Real speedY);
	void setMaxParticles(int maxParticles) { _maxParticles = maxParticles; }
	void setViscosityType(ViscosityType viscosityType) { _viscosityType = viscosityType; }

	void spawnLiquidRectangle(Vec2f position, int width, int height, int type = 0);

	void rotateWall(int wallIdx, float angle, Vec2f orig = Vec2f(0));
	bool isIdxValid(int x, int y);
	void extendGridUpdate(vector<bool> &grid);

private:
	inline tIndex idx1d(const int i, const int j) { return i + j * _resX; }
	void addParticle(const Vec2f& pos, const int type = 0, const Vec2f& vel = Vec2f(0e0), const Vec2f& acc = Vec2f(0e0), const Real press = 0e0, const Real density = 0e0, const Real alpha = 0e0);
	Particle removeParticle(const tIndex index);

	void buildNeighbors();
	void computeDensityAlpha();

	void computeNPforces();
	void adaptDt();
	void predictVel(const Real dt);
	void correctDensityError(const Real dt);
	void updatePos(const Real dt);
	void correctDivergenceError(const Real dt);

	void resizeSSBO();

	shared_ptr<Kernel> _kernel;
	Real _nu, _d0, _m0, _k, _eta, _gamma, _dt;
	Real _resX, _resY;
	Real _h;
	Vec2f _g;
	Real _c;
	const Real DEFAULT_DT = 0.05f;
	const Real MAX_PARTICLE_VEL = 25.0f;

	Vec2f _glasscorner;
	Vec2f _glassSize;
	int _particlesInGlass = 0;
	int _winningGlass = 0;

	tIndex _particleCount;
	tIndex _immovableParticleCount;
	tIndex _immovableGlassParticleCount;

	Vec2f _spawnPosition;
	Real _minDistance = 0.25f;

	vector<Particle> _particleData;
	vector<vector<tIndex>> _neighbors;
	vector<int> _neighborsFlat;
	vector<int> _neighborsCount;
	vector<int> _neighborOffsets;

	vector<vector<tIndex>> _particlesInGrid;

	vector<ParticleGroup> _glassGroups;
	vector<ParticleGroup> _wallGroups;

	Real _moveGlassSpeedX = 4.0f; // per second so dt 1000
	Real _moveGlassSpeedY = 4.0f; // per second so dt 1000

	bool _moveGlassLeft = false;
	bool _moveGlassRight = false;
	bool _moveGlassUp = false;
	bool _moveGlassDown = false;

	ViscosityType _viscosityType = ViscosityType::FLUID;
	
	int _maxParticles = 10000;
	//OpenGL compute shader variables

	GLuint particleBuffer;
	GLuint computeShaderProgramPredictVel;
	GLuint computeShaderProgramDensityAlpha;
	GLuint particleSSBO;

	void setupComputeShaderPredictVel();
	void setupComputeShaderDensityAlpha();
	void setupBuffers();
};

#endif // !_SOLVER_H_
