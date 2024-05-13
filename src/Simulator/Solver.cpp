#include "Solver.h"

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
	_neighbors.clear();

	_particlesInGrid.clear();
	_particlesInGrid.resize(resX * resY);

	_neighbors.clear();
	_particleCount = 0;

	//We add particles in the bottom right corner
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			addParticle(_h*Vec2f(i + 0.25, j + 0.25));
			addParticle(_h*Vec2f(i + 0.75, j + 0.25));
			addParticle(_h*Vec2f(i + 0.25, j + 0.75));
			addParticle(_h*Vec2f(i + 0.75, j + 0.75));
		}
	}
}

void Solver::addParticle(const Vec2f& pos, const Vec2f& vel, const Vec2f& acc, const Real press, const Real density)
{
	_pm.pos.push_back(pos);
	_pm.vel.push_back(vel);
	_pm.acc.push_back(acc);
	_pm.press.push_back(press);
	_pm.density.push_back(density);
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

	_pm.pos.erase(_pm.pos.begin() + index);
	_pm.vel.erase(_pm.vel.begin() + index);
	_pm.acc.erase(_pm.acc.begin() + index);
	_pm.press.erase(_pm.press.begin() + index);
	_pm.density.erase(_pm.density.begin() + index);

	_neighbors.erase(_neighbors.begin() + index);
	_particleCount--;
	return p;
}

void Solver::update(const Real dt) {
	
	buildNeighbors();
	computeDensity();
	computePressure();
	computeViscosity();

	updateVel(dt);
	updatePos(dt);
}

void Solver::buildNeighbors() {
	// We first build the grid
	_particlesInGrid.clear();
	_particlesInGrid.resize(_resX * _resY);
	for (int i = 0; i < _particleCount; i++) {
		int x = _pm.pos[i].x / _h;
		int y = _pm.pos[i].y / _h;
		tIndex idx = idx1d(x, y);
		if (idx >= 0 && idx < _resX * _resY) {
			_particlesInGrid[idx].push_back(i);
		}
	}

	// Then we build the neighbors
	_neighbors.clear();
	_neighbors.resize(_particleCount);
	for (int i = 0; i < _particleCount; i++) {
		int x = _pm.pos[i].x / _h;
		int y = _pm.pos[i].y / _h;
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				tIndex idx = idx1d(x + dx, y + dy);
				if (idx >= 0 && idx < _resX * _resY) {
					for (int j = 0; j < _particlesInGrid[idx].size(); j++) {
						tIndex p = _particlesInGrid[idx][j];
						if (p != i && (_pm.pos[i] - _pm.pos[j]).length() <= _h) {
							_neighbors[i].push_back(p);
						}
					}
				}
			}
		}
	}
}

void Solver::computeDensity() {
	for (int i = 0; i < _particleCount; i++) {
		for (int j = 0; j < _neighbors[i].size(); j++) {
			_pm.density[i] += _m0 * _kernel->W(_pm.pos[i] - _pm.pos[j]);
		}
	}
}

void Solver::computeViscosity(){
	for (int i=0; i<_particleCount; i++){
		for (int j=0; j<_neighbors[i].size(); j++){
			// suppose all masses are equal
			_pm.acc[i] += _nu * (_pm.vel[j]-_pm.vel[i]) / _pm.density[j] * _kernel->laplW(_pm.pos[i] - _pm.pos[j]);
		}
	}
}

void Solver::computePressure(){
	for (int i=0; i<_particleCount; i++){
		for (int j=0; j<_neighbors[i].size(); j++){
			// suppose all masses are equal
			_pm.acc[i] += - (1/_m0) * (_pm.press[i]+_pm.press[j])/(2.0*_pm.density[j]) * _kernel->gradW(_pm.pos[i] - _pm.pos[j]);
		}
	}
}

void Solver::updateVel(const Real dt) {
	for (int i = 0; i < _particleCount; i++) {
		_pm.acc[i] = _g;
		_pm.vel[i] += dt * (_pm.acc[i]);
		//DEBUG("{0}", _pm.vel);
	}
}

void Solver::updatePos(const Real dt) {
	//vector<int> toRemove;
	for (int i = 0; i < _particleCount; i++) {
		_pm.pos[i] += dt * _pm.vel[i];
		/*if (_pm.pos[i].x < 0 || _pm.pos[i].x > _resX * _h || _pm.pos[i].y < 0 || _pm.pos[i].y > _resY * _h) {
			toRemove.push_back(i);
		}*/
	}

	/*for (int i = 0; i < toRemove.size(); i++) {
		removeParticle(toRemove[i]);
	}*/
}