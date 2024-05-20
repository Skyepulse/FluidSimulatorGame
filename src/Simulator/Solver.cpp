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
	for (int i = 1; i < 21; i++) {
		for (int j = 1; j < 21; j++) {
			addParticle(sr*Vec2f(i + 0.25, j + 0.25));
			addParticle(sr*Vec2f(i + 0.75, j + 0.25));
			addParticle(sr*Vec2f(i + 0.25, j + 0.75));
			addParticle(sr*Vec2f(i + 0.75, j + 0.75));
		}
	}
}

void Solver::addParticle(const Vec2f& pos, const int type, const Vec2f& vel, const Vec2f& acc, const Real press, const Real density)
{
	_pm.pos.push_back(pos);
	_pm.vel.push_back(vel);
	_pm.acc.push_back(acc);
	_pm.press.push_back(press);
	_pm.density.push_back(density);
	_pm.type.push_back(type);
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

	_pm.pos.erase(_pm.pos.begin() + index);
	_pm.vel.erase(_pm.vel.begin() + index);
	_pm.acc.erase(_pm.acc.begin() + index);
	_pm.press.erase(_pm.press.begin() + index);
	_pm.density.erase(_pm.density.begin() + index);
	_pm.type.erase(_pm.type.begin() + index);

	_neighbors.erase(_neighbors.begin() + index);
	_particleCount--;
	if(p.type == 1) _immovableParticleCount--;	
	return p;
}

void Solver::init() {
	buildNeighbors();

	initDensity();
}

void Solver::update(const Real dt) {
	computePressure();

	updateVel(dt);
	updatePos(dt);
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

void Solver::initDensity() {
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

}


void Solver::computePressure(){
	for (int i=0; i<_particleCount; i++){
		_pm.press[i] = max(_k*(pow(_pm.density[i]/_d0, 7.0f) - 1.0f), 0.0f);
		//_pm.press[i] = max(_k*(pow(_pm.density[i]/_d0, 7.0f) - 1.0f), 0.0f);
		//CORE_DEBUG("press {0}", _pm.density[i]/_d0);}
		//CORE_DEBUG("press {0}", _pm.press[i]);
	}
	
}


void Solver::updateVel(const Real dt) {
	for (int i = 0; i < _particleCount; i++) {
		if(_pm.type[i] == 1) continue;
		_pm.acc[i] = _g;

		for (int j=0; j<_neighbors[i].size(); j++){
			// suppose all masses are equal
			// pressure force
			tIndex p = _neighbors[i][j];
			if (i == p) continue;

			_pm.acc[i] += - 1.0/_m0 * (_pm.press[i]+_pm.press[p])/(2.0*_pm.density[p]) * _kernel->gradW(_pm.pos[i] - _pm.pos[p]);

			// viscosity force
			//_pm.acc[i] += _nu * (_pm.vel[p]-_pm.vel[i]) / _pm.density[p] * _kernel->laplW(_pm.pos[i] - _pm.pos[p]);

			Vec2f x = (_pm.pos[i] - _pm.pos[p]);
            Vec2f u = (_pm.vel[i] - _pm.vel[p]);
            _pm.acc[i] += 2.0f*_nu * _m0/_pm.density[p] * u  * x.dotProduct(_kernel->gradW(_pm.pos[i] - _pm.pos[p])) / (x.dotProduct(x) + 0.01f*_h*_h);
		}

		_pm.vel[i] += dt * (_pm.acc[i]);
		//DEBUG("{0}", _pm.vel);
	}
}

void Solver::updatePos(const Real dt) {
	//vector<int> toRemove;
	for (int i = 0; i < _particleCount; i++) {
		if(_pm.type[i] == 1) continue;
		_pm.pos[i] += dt * _pm.vel[i];
		/*if (_pm.pos[i].x < 0 || _pm.pos[i].x > _resX * _h || _pm.pos[i].y < 0 || _pm.pos[i].y > _resY * _h) {
			toRemove.push_back(i);
		}*/
	}

	/*for (int i = 0; i < toRemove.size(); i++) {
		removeParticle(toRemove[i]);
	}*/
}