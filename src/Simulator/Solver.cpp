#include "Solver.h"
#include <algorithm>

// TEMP
#include "../Core/Log.h"


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

	for (int i = 0; i < resX; i++) {
		addParticle(sr * Vec2f(i + 0.25,0.25), 1);
		addParticle(sr * Vec2f(i + 0.75,0.25), 1);
		addParticle(sr * Vec2f(i + 0.25,0.75), 1);
		addParticle(sr * Vec2f(i + 0.75,0.75), 1);
		addParticle(sr * Vec2f(i + 0.25, resY-1 + 0.25), 1);
		addParticle(sr * Vec2f(i + 0.75, resY-1 + 0.25), 1);
		addParticle(sr * Vec2f(i + 0.25, resY-1 + 0.75), 1);
		addParticle(sr * Vec2f(i + 0.75, resY-1 + 0.75), 1);
	}

	for (int j = 1; j < resY-1; j++) {
		addParticle(sr * Vec2f(0.25, j + 0.25), 1);
		addParticle(sr * Vec2f(0.75, j + 0.25), 1);
		addParticle(sr * Vec2f(0.25, j + 0.75), 1);
		addParticle(sr * Vec2f(0.75, j + 0.75), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.75, j + 0.25), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.25, j + 0.75), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.75, j + 0.75), 1);
		addParticle(sr * Vec2f(resX - 1 + 0.25, j + 0.25), 1);
	}

	//We add particles in the bottom right corner
	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 10; j++) {
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

	_pm.pos.erase(_pm.pos.begin() + index);
	_pm.vel.erase(_pm.vel.begin() + index);
	_pm.acc.erase(_pm.acc.begin() + index);
	_pm.press.erase(_pm.press.begin() + index);
	_pm.density.erase(_pm.density.begin() + index);
	_pm.type.erase(_pm.type.begin() + index);
	_pm.alpha.erase(_pm.alpha.begin() + index);

	_neighbors.erase(_neighbors.begin() + index);
	_particleCount--;
	if(p.type == 1) _immovableParticleCount--;	
	return p;
}

void Solver::init() {
	buildNeighbors();
	computeDensity();
	computeAlpha();
	CORE_DEBUG("///////////////////////////////////init done/////////////////////////////////");
}

void Solver::update() {
	computeNPforces();
	adaptDt();
	predictVel(_dt);

	correctDensityError(_dt);
	updatePos(_dt);
	buildNeighbors();
	computeDensity();
	computeAlpha();
	correctDivergenceError(_dt);
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
		if (idx >= 0 && idx < _resX * _resY) {
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
		//CORE_DEBUG("neighbors {}", _neighbors[i].size());
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
		Real di = _pm.density[i];
		Vec2f a = Vec2f(0e0);
		Real b = 0e0;
		for (int j = 0; j < _neighbors[i].size(); j++) {
			tIndex p = _neighbors[i][j];
			if(i == p) continue;
			Vec2f factor = _m0 * _kernel->gradW(_pm.pos[i] - _pm.pos[p]);
			b += _pm.type[p] == 1 ? 0 : factor.lengthSquare();
			a += factor;
		}
		_pm.alpha[i] = di/(b + a.lengthSquare());
		//CORE_DEBUG("alpha {}", _pm.alpha[i]);
	}
}

void Solver::computeNPforces() {
	for (int i = 0; i < _particleCount; i++) {
		if(_pm.type[i] == 1) continue;
		// gravity
		_pm.acc[i] = _g;
		//Vec2f debugViscosity = Vec2f(0e0);

		for (int j=0; j<_neighbors[i].size(); j++){
			// suppose all masses are equal
			tIndex p = _neighbors[i][j];
			if (i == p) continue;

			// viscosity force
			Vec2f x = (_pm.pos[i] - _pm.pos[p]);
            Vec2f u = (_pm.vel[i] - _pm.vel[p]);
            _pm.acc[i] += 2.0f*_nu * _m0/_pm.density[p] * u  * x.dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[p])) / (x.dotProduct(x) + 0.01f*_h*_h);
			//debugViscosity += 2.0f*_nu * _m0/_pm.density[p] * u  * x.dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[p])) / (x.dotProduct(x) + 0.01f*_h*_h);
		}
		//CORE_DEBUG("acc {0} {1} {2}", debugViscosity.x, debugViscosity.y, i);
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
		_pm.vel[i] += dt * (_pm.acc[i]);
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
	//DEBUG("dt {}", _dt);
}

void Solver::updatePos(const Real dt) {
	//vector<int> toRemove;
	for (int i = 0; i < _particleCount; i++) {
		if(_pm.type[i] == 1) continue;
		_pm.pos[i] += dt * _pm.vel[i];
	}
}

void Solver::correctDivergenceError(const Real dt){
	vector<Real> dp(_particleCount);
	Real dpAvg = 1000000;
	Real eta = 0.01;
	Real dtInv = 1.0 / dt;
	while (dpAvg > eta){
		dpAvg = 0;
		for (tIndex i=0; i<_particleCount; i++){
			dp[i] = -_pm.density[i] * dtInv * (_pm.vel[i].x + _pm.vel[i].y);
			dpAvg += dp[i];
		}

		for (tIndex i=0; i<_particleCount; i++){
			Real ki = dtInv * dp[i] * _pm.alpha[i];
			for (int p=0; p<_neighbors[i].size(); p++){
				tIndex j = _neighbors[i][p];
				if(i == j) continue;
				Real kj = dtInv * dp[j] * _pm.alpha[j];
				_pm.vel[i] += - dt*_m0 * (ki/_pm.density[i] + kj/_pm.density[j]) * _kernel->gradW(_pm.pos[i] - _pm.pos[j]);
			}
		}
	}
}

void Solver::correctDensityError(const Real dt){
	vector<Real> dens(_particleCount);
	Real densAvg = 100000;
	Real eta = 0.01;
	Real dt2Inv = 1.0 / (dt*dt);

	while (densAvg - _d0 > eta){
		densAvg = 0;
		for (tIndex i=0; i<_particleCount; i++){
			Real factor = 0e0;
			for (int p = 0; p < _neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				if (i == j) continue;
				factor += _m0 * (_pm.vel[i] - _pm.vel[j]).dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[j]));
				//CORE_DEBUG("factor {0} {1}-{2}", _m0 * (_pm.vel[i] - _pm.vel[j]).dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[j])), i, j);
			}
			dens[i] = _pm.density[i] + dt * factor;
			//CORE_DEBUG("dens {} {}", dens[i], i);
			densAvg += dens[i];
		}

		for (tIndex i=0; i<_particleCount; i++){
			Real ki = dt2Inv*(dens[i] - _d0) * _pm.alpha[i];
			for (int p=0; p<_neighbors[i].size(); p++){
				tIndex j = _neighbors[i][p];
				if(i == j) continue;
				Real kj = dt2Inv*(dens[j] - _d0) * _pm.alpha[j];
				_pm.vel[i] += - dt * _m0 * (ki/_pm.density[i] + kj/_pm.density[j]) * _kernel->gradW(_pm.pos[i] - _pm.pos[j]);
				//CORE_DEBUG("vel {} {} | {} {}", _kernel->gradW(_pm.pos[i] - _pm.pos[j]).x, _kernel->gradW(_pm.pos[i] - _pm.pos[j]).y,i, j);
			}
		}

	}
}