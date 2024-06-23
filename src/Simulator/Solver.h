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

enum ViscosityType
{
	FLUID = 0,
	VISCOUS = 1
};

struct Particle {
	Vec2f pos; // position
	Vec2f vel; // velocity
	Vec2f acc; // acceleration
	Real density; // density
	Real alpha; // alpha
	int type; // type of particle
	bool isInGlass; // is particle in glass
	bool needUpdate=true;
	ViscosityType viscosityType;
};


struct ParticleGroup {
	tIndex startIdx;
	tIndex endIdx;
	vector<Vec2f> initPos;
	Vec2f displacement;
	Vec2f vel;

	ParticleGroup() : startIdx(0), endIdx(0), displacement(0), vel(0) {}

    ParticleGroup(tIndex start, tIndex end, const std::vector<Vec2f>& positions)
        : startIdx(start), endIdx(end), initPos(positions), displacement(0), vel(0) {}
};

struct RigidBody: ParticleGroup {
	Vec2f pos;
	Vec2f vel;
	Real relInvMass;

	Real omega; // angular velocity
	Real theta; // angle
	Real invI;

	RigidBody() : ParticleGroup(), pos(0), vel(0), relInvMass(0), omega(0), theta(0), invI(0) {}

    RigidBody(const tIndex start, const tIndex end, const std::vector<Vec2f>& positions, const Vec2f& position, const Real relMass)
        : ParticleGroup(start, end, positions), pos(position), vel(0), relInvMass(1.0/relMass), omega(0), theta(0), invI(0) {}

	RigidBody(const tIndex start, const tIndex end, const std::vector<Vec2f>& positions, const Vec2f& position, const Real relMass, const Real theta_)
        : ParticleGroup(start, end, positions), pos(position), vel(0), relInvMass(1.0/relMass), omega(0), theta(theta_),  invI(0)  {}
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
	void spawnParticle(Vec2f position, ViscosityType viscosityType);

	void drawWalls(int resX, int resY);
	void drawStraightLineWall(const Vec2f& p1, int particleLength, int type = 1, bool save=true);
	void drawAngleLineWall(const Vec2f& p1, int particleLength, Real angle, int type = 1, bool save=true);
	void drawAngleRectangleWall(const Vec2f& p1, int width, int height, Real angle, int type = 1, bool save=true);

	void drawWinningGlass(int width, int height, Vec2f cornerPosition);
	void drawRegularGlass(int width, int height, Vec2f cornerPosition);
	void setSpawnPosition(Vec2f position);

	Vec2f getGlassPosition();
	void setMaxParticles(int maxParticles) { _maxParticles = maxParticles; }

	void spawnLiquidRectangle(Vec2f position, int width, int height, int type = 0, ViscosityType viscosityType = ViscosityType::FLUID);

	void rotateWall(int wallIdx, float angle, Vec2f orig = Vec2f(0));
	void moveWall(int wallIdx, Vec2f moveVector);
	bool isIdxValid(int x, int y);
	void extendGridUpdate(vector<bool> &grid);

	void rotateGlass(int glassIdx, float angle, Vec2f orig = Vec2f(0));
	void moveGlass(int glassIdx, Vec2f moveVector, bool isWinningGlass = false);
	void addRigidBody(Vec2f pos, int width, int height, Real relMass);

	void activateInfiniteWalls() { _infiniteWalls = true; }
	void deactivateInfiniteWalls() { _infiniteWalls = false; }

	void setViscosityForWin(ViscosityType viscosityType) { _specificParticlesWin = true; _specificParticlesViscosity.push_back(viscosityType); }

	Vec2f getGravity() { return _g; }
	void setGravity( Vec2f &g ) { _g = g; }

private:
	inline tIndex idx1d(const int i, const int j) { return i + j * _resX; }
	void addParticle(const Vec2f& pos, const int type = 0, ViscosityType viscosityType = ViscosityType::FLUID, const Vec2f& vel = Vec2f(0e0), const Vec2f& acc = Vec2f(0e0), const Real press = 0e0, const Real density = 0e0, const Real alpha = 0e0);
	Particle removeParticle(const tIndex index);

	void buildNeighbors();
	void computeDensityAlpha();

	void computeNPforces();
	void adaptDt();
	void predictVel(const Real dt);
	void correctDensityError(const Real dt);
	void updatePos(const Real dt);
	void correctDivergenceError(const Real dt);

	void updateRigidBodies(const Real dt);

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
	bool _moveGlassCorner = false;
	Vec2f _winningGlassVel = Vec2f(0);
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

	bool _moveGlassLeft = false;
	bool _moveGlassRight = false;
	bool _moveGlassUp = false;
	bool _moveGlassDown = false;
	
	int _maxParticles = 10000;
	//OpenGL compute shader variables

	GLuint particleBuffer;
	GLuint computeShaderProgramPredictVel;
	GLuint computeShaderProgramDensityAlpha;
	GLuint particleSSBO;

	void setupComputeShaderPredictVel();
	void setupComputeShaderDensityAlpha();
	void setupBuffers();

	bool _infiniteWalls = false;

	bool _specificParticlesWin = false;
	vector<ViscosityType> _specificParticlesViscosity;
public:
	vector<ParticleGroup> _glassGroups;
	vector<ParticleGroup> _wallGroups;

	vector<RigidBody> _rigidBodies;
};

#endif // !_SOLVER_H_
