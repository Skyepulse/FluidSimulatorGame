#include "Solver.h"
#include <algorithm>

// TEMP
#include "../Core/Log.h"
#include <limits.h>

#include "../Core/Time.h"


void Solver::initSimulation(const Real resX, const Real resY)
{
	_resX = resX;
	_resY = resY;
	_pm.pos.clear();
	_pm.vel.clear();
	_pm.acc.clear();
	_pm.press.clear();
	_pm.density.clear();
	_pm.type.clear();
	_neighbors.clear();
	_pm.alpha.clear();

	_particlesInGrid.clear();
	_particlesInGrid.resize(resX * resY);

	_neighbors.clear();
	_particleCount = 0;
	_immovableParticleCount = 0;

	Real sr = _kernel->getSupportRad();

	drawWalls(resX, resY);
	//drawAngleLineWall(Vec2f(0, 7*resY/10), 45, -30);
	//drawAngleLineWall(Vec2f(resX, 4*resY/10), 45, -160);

	drawWinningGlass(10, 10, Vec2f(10, 1));

	//We add particles in the top right corner
	for (int i = 1; i < 9; i++) {
		for (int j = resY-9; j < resY-2; j++) {
			addParticle(sr*Vec2f(i + 0.25, j + 0.25));
			addParticle(sr*Vec2f(i + 0.75, j + 0.25));
			addParticle(sr*Vec2f(i + 0.25, j + 0.75));
			addParticle(sr*Vec2f(i + 0.75, j + 0.75));
		}
	}
}

void Solver::addParticle(const Vec2f& pos, const int type, const Vec2f& vel, const Vec2f& acc, const Real press, const Real density, const Real alpha)
{
	_pm.pos.push_back(pos);
	_pm.vel.push_back(vel);
	_pm.acc.push_back(acc);
	_pm.press.push_back(press);
	_pm.density.push_back(density);
	_pm.type.push_back(type);
	_pm.alpha.push_back(alpha);
	_pm.isInGlass.push_back(false);
	if(type == 1) _immovableParticleCount++;
	_neighbors.push_back(vector<tIndex>());
	_particleCount++;
}

Particle Solver::removeParticle(const tIndex index) //Erase the particles at the end of the simulation stepsize
{
	Particle p;
	p.pos = _pm.pos[index];
	p.vel = _pm.vel[index];
	p.acc = _pm.acc[index];
	p.press = _pm.press[index];
	p.density = _pm.density[index];
	p.type = _pm.type[index];
	p.alpha = _pm.alpha[index];
	p.isInGlass = _pm.isInGlass[index];

	_pm.pos.erase(_pm.pos.begin() + index);
	_pm.vel.erase(_pm.vel.begin() + index);
	_pm.acc.erase(_pm.acc.begin() + index);
	_pm.press.erase(_pm.press.begin() + index);
	_pm.density.erase(_pm.density.begin() + index);
	_pm.type.erase(_pm.type.begin() + index);
	_pm.alpha.erase(_pm.alpha.begin() + index);
	_pm.isInGlass.erase(_pm.isInGlass.begin() + index);

	_neighbors.erase(_neighbors.begin() + index);
	_particleCount--;
	if(p.type == 1 || p.type == 2) _immovableParticleCount--;	
	return p;
}

void Solver::init() {
	buildNeighbors();
	computeDensity();
	computeAlpha();
	CORE_DEBUG("///////////////////////////////////init done/////////////////////////////////");
}

void Solver::update() {
	CORE_DEBUG("///////////////////////////////////UPDATE/////////////////////////////////");	
    CORE_DEBUG("Particle count in glass: {}", _particlesInGlass);
	computeNPforces();
	//CORE_DEBUG("NP forces: {}", Time::GetDeltaTime());
	adaptDt();
	//CORE_DEBUG("Adapt dt: {}", Time::GetDeltaTime());
	predictVel(_dt);
	//CORE_DEBUG("Predict vel {}", Time::GetDeltaTime());

	correctDensityError(_dt);
	//CORE_DEBUG("Correct density: {}", Time::GetDeltaTime());
	updatePos(_dt);
	//CORE_DEBUG("Update pos: {}", Time::GetDeltaTime());
	buildNeighbors();
	//CORE_DEBUG("Build neighbors: {}", Time::GetDeltaTime());
	computeDensity();
	//CORE_DEBUG("Compute density: {}", Time::GetDeltaTime());
	computeAlpha();
	//CORE_DEBUG("Compute alpha: {}", Time::GetDeltaTime());
	correctDivergenceError(_dt);
	//CORE_DEBUG("Correct divergence: {}", Time::GetDeltaTime());
}

void Solver::buildNeighbors() {
	// We first build the grid

	Real R = _kernel->getSupportRad();
	_particlesInGrid.clear();
	_particlesInGrid.resize(_resX * _resY);
	for (int i = 0; i < _particleCount; i++) {
		int x = _pm.pos[i].x / R;
		int y = _pm.pos[i].y / R;
		tIndex idx = idx1d(x, y);
		if (x >= 0 && y >= 0 && x < _resX && y < _resY) {
			_particlesInGrid[idx].push_back(i);
		}
	}

	// Then we build the neighbors
	_neighbors.clear();
	_neighbors.resize(_particleCount);
	for (int i = 0; i < _particleCount; i++) {
		int x = _pm.pos[i].x / R;
		int y = _pm.pos[i].y / R;
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				tIndex idx = idx1d(x + dx, y + dy);
				if (x + dx >= 0 && y + dy >= 0 && x + dx < _resX && y + dy < _resY) {
					for (int j = 0; j < _particlesInGrid[idx].size(); j++) {
						tIndex p = _particlesInGrid[idx][j];
						if ((_pm.pos[i] - _pm.pos[p]).length() <= R) {
							_neighbors[i].push_back(p);
						}
					}
				}
			}
		}
		// CORE_DEBUG("neighbors {} {}", i, _neighbors[i].size());
	}
}

void Solver::computeDensity() {
	for (int i = 0; i < _particleCount; i++) {
		_pm.density[i] = 0;
		for (int j = 0; j < _neighbors[i].size(); j++) {
			tIndex p = _neighbors[i][j];
			_pm.density[i] += _m0 * _kernel->W(_pm.pos[i] - _pm.pos[p]);

		//CORE_DEBUG("dens {} {}", i, _kernel->W(_pm.pos[i] - _pm.pos[p]));
		}
		//CORE_DEBUG("dens {}", _pm.density[i]);
	}
}

void Solver::computeAlpha() {
	for (int i = 0; i < _particleCount; i++) {
		Vec2f a = Vec2f(0e0);
		Real b = 0e0;
		for (int j = 0; j < _neighbors[i].size(); j++) {
			tIndex p = _neighbors[i][j];
			Vec2f factor = _m0 * _kernel->gradW(_pm.pos[i] - _pm.pos[p]);
			b += _pm.type[p] == 1 ? 0 : factor.lengthSquare();
			a += factor;
			//b += factor.lengthSquare();
		}

		_pm.alpha[i] = 1.0/max(b + a.lengthSquare(), 1.0e-6f);
		//CORE_DEBUG("alpha {}", _pm.alpha[i]);
	}
}

void Solver::computeNPforces() {
	for (int i = 0; i < _particleCount; i++) {
		if(_pm.type[i] == 1 || _pm.type[i] == 2) continue;
		// gravity
		_pm.acc[i] = _g;
		//_pm.acc[i] = Vec2f(0e0);
		//Vec2f debugViscosity = Vec2f(0e0);

		for (int j=0; j<_neighbors[i].size(); j++){
			// suppose all masses are equal
			tIndex p = _neighbors[i][j];

			// viscosity force
			Vec2f x = (_pm.pos[i] - _pm.pos[p]);
            Vec2f u = (_pm.vel[i] - _pm.vel[p]);
            _pm.acc[i] += 2.0f*_nu * _m0/_pm.density[p] * u  * x.dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[p])) / (x.dotProduct(x) + 0.01f*_h*_h);
			//debugViscosity += 2.0f*_nu * _m0/_pm.density[p] * u  * x.dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[p])) / (x.dotProduct(x) + 0.01f*_h*_h);
		}
		//CORE_DEBUG("viscosity debug term {0} {1} {2}", debugViscosity.x, debugViscosity.y, i);
	}
}


void Solver::computePressure(){
	for (int i=0; i<_particleCount; i++){
		_pm.press[i] = max(_k*(pow(_pm.density[i]/_d0, 7.0f) - 1.0f), 0.0f);
		//_pm.press[i] = max(_k*(pow(_pm.density[i]/_d0, 7.0f) - 1.0f), 0.0f);
		//CORE_DEBUG("press {0}", _pm.density[i]/_d0);}
		//CORE_DEBUG("press {0}", _pm.press[i]);
	}
}

void Solver::predictVel(const Real dt){
	for (int i = 0; i < _particleCount; i++) {
		if (_pm.type[i] == 0)
			_pm.vel[i] += dt * (_pm.acc[i]);
		//CORE_DEBUG("Predicted vel for particle {0} : {1} {2}", i, _pm.vel[i].x, _pm.vel[i].y);
	}
}

void Solver::adaptDt() {
	Real maxVel2 = 0e0;
	for (int i = 0; i < _particleCount; i++) {
		maxVel2 = max(maxVel2, _pm.vel[i].lengthSquare());
	}
	Real maxVel = sqrt(maxVel2);
	if(maxVel == 0e0) _dt = DEFAULT_DT;
	else _dt = 0.4*_h/maxVel;
	_dt = min(_dt, DEFAULT_DT);
	//CORE_DEBUG("dt {}", _dt);
}

void Solver::updatePos(const Real dt) {
	//vector<int> toRemove;
	for (int i = 0; i < _particleCount; i++) {
		if(_pm.type[i] == 1 || _pm.type[i] == 2) continue;
		_pm.pos[i] += dt * _pm.vel[i];
		//CORE_DEBUG("vel {0} {1} {2} {3} {4} {5}", dt*_pm.vel[i].x, dt*_pm.vel[i].y, i, dt, _pm.vel[i].x, _pm.vel[i].y);

		//We check if the particle is inside the glass
		if (_pm.pos[i].x >= _glasscorner.x && _pm.pos[i].x <= _glasscorner.x + _glassSize.x && _pm.pos[i].y >= _glasscorner.y && _pm.pos[i].y <= _glasscorner.y + _glassSize.y) {
			if (!_pm.isInGlass[i]) _particlesInGlass++;
			_pm.isInGlass[i] = true;
		}
		else {
			if (_pm.isInGlass[i]) _particlesInGlass--;
			_pm.isInGlass[i] = false;
		}	
	}
}

void Solver::correctDivergenceError(const Real dt){
	vector<Real> dp(_particleCount);
	Real dpAvg = (Real)INT_MAX;
	Real eta = 10.0f;
	Real dtInv = (Real)1.0f / dt;

	int iter = 0;

	while ((abs(dpAvg) > eta || iter < 1) && iter < 10){
		//CORE_DEBUG("dpAvg {}", dpAvg);
		dpAvg = 0.0f;
		for (tIndex i=0; i<_particleCount; i++){
			dp[i] = 0e0f;
			for (int p = 0; p < _neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				dp[i] += _m0 * (_pm.vel[i] - _pm.vel[j]).dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[j]));
				//CORE_DEBUG("factor {0} {1}-{2}", _m0 * (_pm.vel[i] - _pm.vel[j]).dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[j])), i, j);
			}
			if (_pm.type[i] == 0) dpAvg += dp[i];
		}
		dpAvg /= _particleCount;

		for (tIndex i=0; i<_particleCount; i++){
			if (_pm.type[i] == 1) continue;
			Real ki = dtInv * dp[i] * _pm.alpha[i];
			for (int p=0; p<_neighbors[i].size(); p++){
				tIndex j = _neighbors[i][p];
				Real kj = dtInv * dp[j] * _pm.alpha[j];
				_pm.vel[i] += -dt*_m0 * (ki/_pm.density[i] + kj/_pm.density[j]) * _kernel->gradW(_pm.pos[i] - _pm.pos[j]);
			}
		}
		// CORE_DEBUG("dpAvg {}", dpAvg);
		iter++;
	}
	// CORE_DEBUG("Final dpAvg {}", dpAvg);
}

void Solver::correctDensityError(const Real dt){
	vector<Real> dens(_particleCount);
	Real densAvg = (Real)INT_MAX;
	Real eta = 0.01 * 0.01 * _d0;
	Real dt2Inv = (Real)1.0f / (dt*dt);

	int iter = 0;

	float firstCount=0;
	float secondCount=0;

	while ((iter < 2 || abs(densAvg - _d0) > eta) && iter < 10){
		Time::GetDeltaTime();
		//CORE_DEBUG("densAvg {} {}", densAvg, _d0);
		densAvg = 0.0f;
		for (tIndex i=0; i<_particleCount; i++){
			Real factor = 0e0f;
			for (int p = 0; p < _neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				factor += (_pm.vel[i] - _pm.vel[j]).dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[j]));
				//CORE_DEBUG("factor {0} {1}-{2}", _m0 * (_pm.vel[i] - _pm.vel[j]).dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[j])), i, j);
			}
			dens[i] = max(_pm.density[i] + dt * _m0 * factor, _d0);
			//CORE_DEBUG("factor {} {}", factor, i);
			//CORE_DEBUG("dens {} {}", dens[i], i);
			if (_pm.type[i] == 0) densAvg += dens[i];
		}
		densAvg /= _particleCount;

		firstCount += Time::GetDeltaTime();

		for (tIndex i=0; i<_particleCount; i++){
			if (_pm.type[i] == 1 || _pm.type[i] == 2) continue;
			Real ki = max(dt2Inv*(dens[i] - _d0) * _pm.alpha[i], 0.0f);
			for (int p=0; p<_neighbors[i].size(); p++){
				tIndex j = _neighbors[i][p];
				Real kj = max(dt2Inv*(dens[j] - _d0) * _pm.alpha[j], 0.0f);
				_pm.vel[i] += - dt * _m0 * (ki + kj) * _kernel->gradW(_pm.pos[i] - _pm.pos[j]);
				//CORE_DEBUG("vel {} {} | {} {}", _kernel->gradW(_pm.pos[i] - _pm.pos[j]).x, _kernel->gradW(_pm.pos[i] - _pm.pos[j]).y,i, j);
			}
		}
		iter++;
		secondCount += Time::GetDeltaTime();
	}
	//CORE_DEBUG("Final densAvg {} {}", densAvg, _d0);

	CORE_DEBUG("First: {}, Second: {}", firstCount, secondCount);
}

void Solver::drawWalls(int resX, int resY) {
	Real sr = _kernel->getSupportRad();
	for (int i = 0; i < resX; i++) {
		addParticle(sr * Vec2f(i + 0.25, 0.25), 1);
		addParticle(sr * Vec2f(i + 0.75, 0.25), 1);
		addParticle(sr * Vec2f(i + 0.25, 0.75), 1);
		addParticle(sr * Vec2f(i + 0.75, 0.75), 1);
		addParticle(sr * Vec2f(i + 0.25, resY - 1 + 0.25), 1);
		addParticle(sr * Vec2f(i + 0.75, resY - 1 + 0.25), 1);
		addParticle(sr * Vec2f(i + 0.25, resY - 1 + 0.75), 1);
		addParticle(sr * Vec2f(i + 0.75, resY - 1 + 0.75), 1);
	}

	for (int j = 1; j < resY - 1; j++) {
		addParticle(sr * Vec2f(0.25, j + 0.25), 1);
		addParticle(sr * Vec2f(0.75, j + 0.25), 1);
		addParticle(sr * Vec2f(0.25, j + 0.75), 1);
		addParticle(sr * Vec2f(0.75, j + 0.75), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.75, j + 0.25), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.25, j + 0.75), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.75, j + 0.75), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.25, j + 0.25), 1);
	}
}

void Solver::drawStraightLineWall(const Vec2f& p1, int particleLength, int type) {
	Real sr = _kernel->getSupportRad(); 
	for (int i = 0; i < particleLength; i++) {
		Vec2f pos1 = p1 + Vec2f(0.25, 0.25) + Vec2f(0.5, 0.0) * i;
		addParticle(sr * pos1, type);
		Vec2f pos2 = p1 + Vec2f(0.25, 0.75) + Vec2f(0.5, 0.0) * i;
		addParticle(sr * pos2, type);
	}
}

void Solver::drawAngleLineWall(const Vec2f& p1, int particleLength, Real angle, int type) {
	Real sr = _kernel->getSupportRad();
	Real radAngle = angle * M_PI / 180.0;
	Real cosAngle = cos(radAngle);
	Real sinAngle = sin(radAngle);


	//Turn the line by angle
	for (int i = 0; i < particleLength; i++) {
		Vec2f pos1 = p1 + Vec2f(0.25, 0.25) + Vec2f(0.5, 0.0) * i;
		Vec2f newPos1 = Vec2f(cosAngle * (pos1.x - p1.x) - sinAngle * (pos1.y - p1.y) + p1.x, sinAngle * (pos1.x - p1.x) + cosAngle * (pos1.y - p1.y) + p1.y);
		addParticle(sr * newPos1, type);

		Vec2f pos2 = p1 + Vec2f(0.25, 0.75) + Vec2f(0.5, 0.0) * i;
		Vec2f newPos2 = Vec2f(cosAngle * (pos2.x - p1.x) - sinAngle * (pos2.y - p1.y) + p1.x, sinAngle * (pos2.x - p1.x) + cosAngle * (pos2.y - p1.y) + p1.y);
		addParticle(sr * newPos2, type);	
	}
}

void Solver::drawWinningGlass(int width, int height, Vec2f cornerPosition) {
	drawAngleLineWall(cornerPosition, width, 0, 2);
	drawAngleLineWall(cornerPosition + Vec2f(1.0f), height, 90, 2);
	drawAngleLineWall(cornerPosition + Vec2f(width/2, 1.0f), height, 90, 2);
	this->_glasscorner = cornerPosition;
	this->_glassSize = Vec2f(width/2, height/2);
}