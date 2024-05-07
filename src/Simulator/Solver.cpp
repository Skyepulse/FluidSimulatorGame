#include "Solver.h"
#include <math.h>
#include <memory>

void Solver::initSimulation(const Real resX, const Real resY)
{
	_resX = resX;
	_resY = resY;
	_pm.pos.clear();
	_pm.vel.clear();
	_pm.acc.clear();
	_pm.press.clear();
	_pm.density.clear();

	_particlesInGrid.clear();
	_particlesInGrid.resize(resX * resY);

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
			_pm.acc[i] += - (_pm.press[i]+_pm.press[j])/(2.0*_pm.density[j]) * _kernel->gradW(_pm.pos[i] - _pm.pos[j]);
		}
	}
}