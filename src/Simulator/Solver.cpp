#include "Solver.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

// TEMP
#include "../Core/Log.h"
#include <limits.h>

#include "../Core/Time.h"


void Solver::initSimulation(const Real resX, const Real resY)
{
	_resX = resX;
	_resY = resY;

	_particleData.clear();

	_wallGroups.clear();
	_glassGroups.clear();
	_rigidBodies.clear();

	_neighbors.clear();

	_particlesInGrid.clear();
	_particlesInGrid.resize(resX * resY);

	_particleCount = 0;
	_immovableParticleCount = 0;
	_immovableGlassParticleCount = 0;
}

void Solver::addParticle(const Vec2f& pos, const int type, ViscosityType viscosityType ,const Vec2f& vel, const Vec2f& acc, const Real press, const Real density, const Real alpha)
{
	Particle p;
	p.pos = pos;
	p.vel = vel;
	p.acc = acc;
	p.density = density;
	p.type = type;
	p.alpha = alpha;
	p.isInGlass = false;
	p.viscosityType = viscosityType;


	_particleData.push_back(p);
	_neighbors.push_back(vector<tIndex>());

	if(type == 1) _immovableParticleCount++;
	if(type == 2) _immovableGlassParticleCount++;
	_particleCount++;

	resizeSSBO();
}

Particle Solver::removeParticle(const tIndex index) //Erase the particles at the end of the simulation stepsize
{
	Particle p;
	p = _particleData[index];
	_particleData.erase(_particleData.begin() + index);	
	_neighbors.erase(_neighbors.begin() + index);
	_particleCount--;
	if(p.type == 1) _immovableParticleCount--;
	if(p.type == 2) _immovableGlassParticleCount--;

	resizeSSBO();

	return p;

}

void Solver::init() {
	buildNeighbors();
	computeDensityAlpha();
	//CORE_DEBUG("///////////////////////////////////init done/////////////////////////////////");
}

Real Solver::update() {
	//CORE_DEBUG("///////////////////////////////////UPDATE/////////////////////////////////");	
    //CORE_DEBUG("Particle count in glass: {}", _particlesInGlass);
	computeNPforces();
	//CORE_DEBUG("NP forces: {}", Time::GetDeltaTime());
	adaptDt();
	//CORE_DEBUG("Adapt dt: {}", Time::GetDeltaTime());
	predictVel(_dt);
	//CORE_DEBUG("Predict vel {}", Time::GetDeltaTime());
	correctDensityError(_dt);
	//CORE_DEBUG("Correct density: {}", Time::GetDeltaTime());
	updatePos(_dt);
	updateRigidBodies(_dt);
	//CORE_DEBUG("Update pos: {}", Time::GetDeltaTime());
	buildNeighbors();
	//CORE_DEBUG("Build neighbors: {}", Time::GetDeltaTime());
	computeDensityAlpha();
	//CORE_DEBUG("Compute density and alpha: {}", Time::GetDeltaTime());
	correctDivergenceError(_dt);
	//CORE_DEBUG("Correct divergence: {}", Time::GetDeltaTime());
	return _dt;
}

bool Solver::isIdxValid(int x, int y){
	return (x >= 0 && y >= 0 && x < _resX && y < _resY);
}

void Solver::extendGridUpdate(vector<bool> &grid){
	vector<bool> orig(grid);

	for (int x=0; x<_resX; x++){
		for (int y=0; y<_resY; y++){
			tIndex idx = idx1d(x, y);
			if (!orig[idx]) continue;
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (isIdxValid(x+dx, y+dy)) {
						grid[idx1d(x + dx, y + dy)] = true;
					}
				}
			}
		}
	}
}

void Solver::buildNeighbors() {
	// We first build the grid
	vector<bool> gridNeedUpdate(_resX * _resY, false);

	int count = 0;

	Real R = _kernel->getSupportRad();
	_particlesInGrid.clear();
	_particlesInGrid.resize(_resX * _resY);

	bool checkingWalls = false;

	for (int i = _particleCount-1; i >= 0; i--) {
		int x = _particleData[i].pos.x / R;
		int y = _particleData[i].pos.y / R;
		tIndex idx = idx1d(x, y);
		if (isIdxValid(x, y)) {
			_particleData[i].needUpdate = true;
			if (_particleData[i].type == 0 || _particleData[i].type == 3){
				gridNeedUpdate[idx] = true;
			} else {
				if (!checkingWalls){
					extendGridUpdate(gridNeedUpdate);
					checkingWalls = true;
				}
				if (!gridNeedUpdate[idx]) _particleData[i].needUpdate = false;
			}
			_particlesInGrid[idx].push_back(i);
		} else {
			_particleData[i].needUpdate = false;
		}
	}

	_neighbors.clear();
	_neighbors.resize(_particleCount);


	// Then we build the neighbors
	for (int i = 0; i < _particleCount; i++) {
		if (!_particleData[i].needUpdate) continue;
		int x = _particleData[i].pos.x / R;
		int y = _particleData[i].pos.y / R;
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				tIndex idx = idx1d(x + dx, y + dy);
				if (isIdxValid(x+dx, y+dy)) {
					for (int j = 0; j < _particlesInGrid[idx].size(); j++) {
						tIndex p = _particlesInGrid[idx][j];
						if ((_particleData[i].pos - _particleData[p].pos).length() <= R) {
							_neighbors[i].push_back(p);
						}
					}
				}
			}
		}
	}
}

void Solver::computeDensityAlpha() {
	for (int i = 0; i < _particleCount; i++) {
		if (!_particleData[i].needUpdate) continue;
		_particleData[i].density = 0;
		Vec2f a = Vec2f(0e0);
		Real b = 0e0;

		for (int j = 0; j < _neighbors[i].size(); j++) {
			tIndex p = _neighbors[i][j];
			_particleData[i].density += _m0 * _kernel->W(_particleData[i].pos - _particleData[p].pos);

			Vec2f factor = _m0 * _kernel->gradW(_particleData[i].pos - _particleData[p].pos);
			b += factor.lengthSquare();
			a += factor;
		}
		_particleData[i].alpha = 1.0 / max(b + a.lengthSquare(), 1.0e-6f);
	}
}

void Solver::computeNPforces() {
	Real visc = 2.0f;
	for (int i = 0; i < _particleCount; i++) {
		if (!_particleData[i].needUpdate) continue;
		if(_particleData[i].type == 1 || _particleData[i].type == 2 || _particleData[i].type == 3) continue;
		// gravity
		_particleData[i].acc = _g;

		for (int j=0; j<_neighbors[i].size(); j++) {
			// suppose all masses are equal
			tIndex p = _neighbors[i][j];

			// viscosity force
			Vec2f x = (_particleData[i].pos - _particleData[p].pos);
            Vec2f u = (_particleData[i].vel - _particleData[p].vel);
			visc = _particleData[i].viscosityType == ViscosityType::VISCOUS ? 50.0f : 2.0f;
            _particleData[i].acc += visc * _nu * _m0 / _particleData[p].density * u * x.dotProduct(_kernel->gradW(_particleData[i].pos - _particleData[p].pos)) / (x.dotProduct(x) + 0.01f * _h * _h);
		}
	}
}

void Solver::predictVel(const Real dt){

	for (auto &glass: _glassGroups){
		for (int i=glass.startIdx; i<glass.endIdx; i++){
			if (this->_moveGlassRight) {
				_particleData[i].vel = _moveGlassSpeedX * Vec2f(1.0f, 0.0f);
			}
			else if (this->_moveGlassLeft) {
				_particleData[i].vel = _moveGlassSpeedX * Vec2f(-1.0f, 0.0f);
			}
			else if (this->_moveGlassUp) {
				_particleData[i].vel = _moveGlassSpeedY * Vec2f(0.0f, 1.0f);
			}
			else if (this->_moveGlassDown) {
				_particleData[i].vel = _moveGlassSpeedY * Vec2f(0.0f, -1.0f);
			}
			else {
				_particleData[i].vel = Vec2f(0.0f, 0.0f);
			}
		}
	}

	for (int i = 0; i < _particleCount; i++) {
		if (_particleData[i].type == 0)
			_particleData[i].vel += dt * (_particleData[i].acc);
	}

	for (auto &rBody: _rigidBodies) rBody.vel += dt * _g;
}

void Solver::adaptDt() {
	Real maxVel2 = 0e0;
	for (int i = 0; i < _particleCount; i++) {
		if (!_particleData[i].needUpdate) continue;
		Real particleVel2 = _particleData[i].vel.lengthSquare();
		if (particleVel2 > MAX_PARTICLE_VEL * MAX_PARTICLE_VEL) {
			_particleData[i].vel *= MAX_PARTICLE_VEL / sqrt(particleVel2);
			particleVel2 = MAX_PARTICLE_VEL * MAX_PARTICLE_VEL;
		}
		maxVel2 = max(maxVel2, particleVel2);
	}
	Real maxVel = sqrt(maxVel2);
	//CORE_DEBUG("Max velocity: {}", maxVel);
	if(maxVel == 0e0) _dt = DEFAULT_DT;
	else _dt = 0.6*_h/maxVel;
	_dt = min(_dt, DEFAULT_DT);
}

void Solver::updatePos(const Real dt) {
	Vec2f glassVel = Vec2f(0.0f, 0.0f);
	for (int i = _particleCount - 1; i >= 0; i--) {
		if(_particleData[i].type == 1 || _particleData[i].type == 3) continue;
		if (_particleData[i].type == 2) {
			_particleData[i].pos += dt * _particleData[i].vel;
			glassVel = _particleData[i].vel;
			continue;
		}
		_particleData[i].pos += dt * _particleData[i].vel;

		//We check if the particle is out of bounds
		Vec2f pos = _particleData[i].pos;
		if (pos.x < 0 || pos.x >= _resX * _kernel->getSupportRad() || pos.y < 0 || pos.y >= _resY * _kernel->getSupportRad()) {
			removeParticle(i);
			continue;
		}

		//We check if the particle is inside the glass
		if (_particleData[i].pos.x >= _glasscorner.x && _particleData[i].pos.x <= _glasscorner.x + _glassSize.x && _particleData[i].pos.y >= _glasscorner.y && _particleData[i].pos.y <= _glasscorner.y + _glassSize.y) {
			if (!_particleData[i].isInGlass) _particlesInGlass++;
			_particleData[i].isInGlass = true;
		}
		else {
			if (_particleData[i].isInGlass) _particlesInGlass--;
			_particleData[i].isInGlass = false;
		}	
	}
	_glasscorner += dt * glassVel;
}

void Solver::updateRigidBodies(const Real dt){
	for (auto &rBody: _rigidBodies){
		Vec2f bufferMov(0);
		Real bufferRot = 0;

		for (int i=rBody.startIdx; i<rBody.endIdx; i++){
			bufferMov += _particleData[i].vel;
			bufferRot += (_particleData[i].pos - rBody.pos).crossProduct(_particleData[i].vel);
		}
		bufferRot *= rBody.invI;

		rBody.omega += bufferRot;
		rBody.theta += rBody.omega * dt;

		rBody.vel += bufferMov * rBody.relInvMass;
		rBody.pos += rBody.vel * dt;
		for (int i=rBody.startIdx; i<rBody.endIdx; i++){
			_particleData[i].vel = Vec2f(0);
			_particleData[i].pos = rBody.pos + rBody.initPos[i-rBody.startIdx].rotated(rBody.theta);
		}
	}
}

void Solver::correctDivergenceError(const Real dt){
	vector<Real> dp(_particleCount);
	Real dpAvg = (Real)INT_MAX;
	Real eta = 10.0f;
	Real dtInv = (Real)1.0f / dt;

	int iter = 0;

	for (int i=0; i<_particleCount; i++){
		if (!_particleData[i].needUpdate) continue;
		_particleData[i].alpha *= dtInv;
	}

	while ((abs(dpAvg) > eta || iter < 1) && iter < 10){
		dpAvg = 0.0f;
		for (tIndex i=0; i<_particleCount; i++){
			if (!_particleData[i].needUpdate) continue;
			dp[i] = 0e0f;
			for (int p = 0; p < _neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				dp[i] += (_particleData[i].vel - _particleData[j].vel).dotProduct(_kernel->gradW(_particleData[i].pos - _particleData[j].pos));
			}
			if (_particleData[i].type == 0) dpAvg += _m0 * dp[i];
		}
		dpAvg /= _particleCount;

		for (tIndex i=0; i<_particleCount; i++){
			if (_particleData[i].type == 1 || _particleData[i].type == 2) continue;
			Real ki = dp[i] * _particleData[i].alpha;
			Vec2f sum(0);
			for (int p=0; p<_neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				Real kj = dp[j] * _particleData[j].alpha;
				sum += (ki/_particleData[i].density + kj/_particleData[j].density) * _kernel->gradW(_particleData[i].pos - _particleData[j].pos);
			}
			_particleData[i].vel += -dt*_m0 * sum;
		}
		iter++;
	}

	for (int i=0; i<_particleCount; i++){
		if (!_particleData[i].needUpdate) continue;
		_particleData[i].alpha *= dt;
	}
}

void Solver::correctDensityError(const Real dt){
	vector<Real> dens(_particleCount);
	Real densAvg = (Real)INT_MAX;
	Real eta = 0.01 * 0.01 * _d0;
	Real dt2Inv = (Real)1.0f / (dt*dt);

	int iter = 0;

	float firstCount=0;
	float secondCount=0;

	for (int i=0; i<_particleCount; i++){
		if (!_particleData[i].needUpdate) continue;
		_particleData[i].alpha *= dt2Inv;
	}

	while ((iter < 2 || abs(densAvg - _d0) > eta) && iter < 10){
		Time::GetDeltaTime();
		densAvg = 0.0f;
		for (tIndex i=0; i<_particleCount; i++){
			if (!_particleData[i].needUpdate) continue;
			Real factor = 0e0f;
			for (int p = 0; p < _neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				factor += (_particleData[i].vel - _particleData[j].vel).dotProduct(_kernel->gradW(_particleData[i].pos - _particleData[j].pos));
			}
			dens[i] = max(_particleData[i].density + dt * _m0 * factor, _d0);
			if (_particleData[i].type == 0) densAvg += dens[i];
		}
		densAvg /= _particleCount;

		firstCount += Time::GetDeltaTime();

		for (tIndex i=0; i<_particleCount; i++){
			if (_particleData[i].type == 1 || _particleData[i].type == 2) continue;
			Real ki = max((dens[i] - _d0) * _particleData[i].alpha, 0.0f);
			Vec2f sum(0);
			for (int p=0; p<_neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				Real kj = max((dens[j] - _d0) * _particleData[j].alpha, 0.0f);
				sum += (ki + kj) * _kernel->gradW(_particleData[i].pos - _particleData[j].pos);
			}
			_particleData[i].vel += -dt * _m0 * sum;
		}


		iter++;
		secondCount += Time::GetDeltaTime();
	}

	for (int i=0; i<_particleCount; i++){
		if (!_particleData[i].needUpdate) continue;
		_particleData[i].alpha *= dt*dt;
	}
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

void Solver::drawStraightLineWall(const Vec2f& p1, int particleLength, int type, bool save) {
	tIndex start = _particleCount;

	Real sr = _kernel->getSupportRad(); 
	for (int i = 0; i < particleLength; i++) {
		Vec2f pos1 = p1 + Vec2f(0.25, 0.25) + Vec2f(0.5, 0.0) * i;
		addParticle(sr * pos1, type);
		Vec2f pos2 = p1 + Vec2f(0.25, 0.75) + Vec2f(0.5, 0.0) * i;
		addParticle(sr * pos2, type);
	}

	if (save){
		ParticleGroup pGroup(start, _particleCount, vector<Vec2f>(_particleCount - start));
		for (int i=start; i<_particleCount; i++){
			pGroup.initPos[i - start] = _particleData[i].pos;
		}
		_wallGroups.push_back(pGroup);
	}
}

void Solver::drawAngleLineWall(const Vec2f& p1, int particleLength, Real angle, int type, bool save) {
	Real sr = _kernel->getSupportRad();
	Real radAngle = angle * M_PI / 180.0;
	Real cosAngle = cos(radAngle);
	Real sinAngle = sin(radAngle);

	tIndex start = _particleCount;

	//Turn the line by angle
	for (int i = 0; i < particleLength; i++) {
		Vec2f pos1 = p1 + Vec2f(0.25, 0.25) + Vec2f(0.5, 0.0) * i;
		Vec2f newPos1 = Vec2f(cosAngle * (pos1.x - p1.x) - sinAngle * (pos1.y - p1.y) + p1.x, sinAngle * (pos1.x - p1.x) + cosAngle * (pos1.y - p1.y) + p1.y);
		addParticle(sr * newPos1, type);

		Vec2f pos2 = p1 + Vec2f(0.25, 0.75) + Vec2f(0.5, 0.0) * i;
		Vec2f newPos2 = Vec2f(cosAngle * (pos2.x - p1.x) - sinAngle * (pos2.y - p1.y) + p1.x, sinAngle * (pos2.x - p1.x) + cosAngle * (pos2.y - p1.y) + p1.y);
		addParticle(sr * newPos2, type);	
	}
	if (save){
		ParticleGroup pGroup(start, _particleCount, vector<Vec2f>(_particleCount - start));
		for (int i=start; i<_particleCount; i++){
			pGroup.initPos[i - start] = _particleData[i].pos;
		}
		_wallGroups.push_back(pGroup);
	}
}

void Solver::drawAngleRectangleWall(const Vec2f& p1, int width, int height, Real angle, int type, bool save) {
	Real sr = _kernel->getSupportRad();
	Real radAngle = angle * M_PI / 180.0;
	Real cosAngle = cos(radAngle);
	Real sinAngle = sin(radAngle);

	tIndex start = _particleCount;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Vec2f pos = p1 + Vec2f(0.25, 0.25) + Vec2f(0.5, 0.0) * i + Vec2f(0.0, 0.5) * j;
			Vec2f newPos = Vec2f(cosAngle * (pos.x - p1.x) - sinAngle * (pos.y - p1.y) + p1.x, sinAngle * (pos.x - p1.x) + cosAngle * (pos.y - p1.y) + p1.y);
			addParticle(sr * newPos, type);
		}
	}
	if (save){
		ParticleGroup pGroup(start, _particleCount, vector<Vec2f>(_particleCount - start));
		for (int i=start; i<_particleCount; i++){
			pGroup.initPos[i - start] = _particleData[i].pos;
		}
		_wallGroups.push_back(pGroup);
	}
}

void Solver::drawWinningGlass(int width, int height, Vec2f cornerPosition) {
	tIndex start = _particleCount;
	drawAngleLineWall(cornerPosition, width, 0, 2, false);
	drawAngleLineWall(cornerPosition + Vec2f(1.0f), height, 90, 2, false);
	drawAngleLineWall(cornerPosition + Vec2f(width/2, 1.0f), height, 90, 2, false);
	this->_glasscorner = cornerPosition;
	this->_glassSize = Vec2f(width/2, height/2);
	this->_winningGlass = (width - 2) * (height - 1) / 2;


	ParticleGroup pGroup(start, _particleCount, vector<Vec2f>(_particleCount - start));
	for (int i=start; i<_particleCount; i++){
		pGroup.initPos[i - start] = _particleData[i].pos;
	}
	_glassGroups.push_back(pGroup);
}

void Solver::drawRegularGlass(int width, int height, Vec2f cornerPosition) {
	tIndex start = _particleCount;
	drawAngleLineWall(cornerPosition, width, 0, 2, false);
	drawAngleLineWall(cornerPosition + Vec2f(1.0f), height, 90, 2, false);
	drawAngleLineWall(cornerPosition + Vec2f(width / 2, 1.0f), height, 90, 2, false);

	ParticleGroup pGroup(start, _particleCount, vector<Vec2f>(_particleCount - start));
	for (int i = start; i < _particleCount; i++) {
		pGroup.initPos[i - start] = _particleData[i].pos;
	}
	_glassGroups.push_back(pGroup);
}

void Solver::spawnParticle(Vec2f position, ViscosityType viscosityType) {
	if(_maxParticles == 0) return;
	//We check the potential neighbors for this particle. If we find a neighbor too close, we don't spawn the particle
	int x = position.x;
	int y = position.y;
	Real sr = _kernel->getSupportRad();
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			tIndex idx = idx1d(x + dx, y + dy);
			if (x + dx >= 0 && y + dy >= 0 && x + dx < _resX && y + dy < _resY) {
				for (int j = 0; j < _particlesInGrid[idx].size(); j++) {
					tIndex p = _particlesInGrid[idx][j];
					if ((position - _particleData[p].pos).length() <= sr/2) {
						return;
					}
				}
			}
		}
	}
	addParticle(position, 0, viscosityType);
	_maxParticles--;
}

void Solver::setSpawnPosition(Vec2f position) {
	_spawnPosition = this->_kernel->getSupportRad()*position;
}

void Solver::spawnLiquidRectangle(Vec2f position, int width, int height, int type, ViscosityType viscosityType) {
	Real sr = _kernel->getSupportRad();
	for (int i = position.x; i < position.x + width; i++) {
		for (int j = position.y; j < position.y + height; j++) {
			addParticle(sr * Vec2f(i + 0.25, j + 0.25), 0, viscosityType);
			addParticle(sr * Vec2f(i + 0.75, j + 0.25), 0, viscosityType);
			addParticle(sr * Vec2f(i + 0.25, j + 0.75), 0, viscosityType);
			addParticle(sr * Vec2f(i + 0.75, j + 0.75), 0, viscosityType);
		}
	}
}

void Solver::addRigidBody(Vec2f pos, int width, int height, Real relMass) {
	tIndex start = _particleCount;
	drawAngleLineWall(pos, width, 0, 3, false);
	drawAngleLineWall(pos + Vec2f(1.0f), height, 90, 3, false);
	drawAngleLineWall(pos + Vec2f(width/2, 1.0f), height, 90, 3, false);
	drawAngleLineWall(pos + Vec2f(1.0f, height/2), width-4, 0, 3, false);

	if (relMass == 0) relMass = _particleCount - start;

	pos += Vec2f(width/4, height/4);

	RigidBody rBody(start, _particleCount, vector<Vec2f>(_particleCount - start), pos, relMass);
	for (int i=start; i<_particleCount; i++){
		rBody.initPos[i - start] = _particleData[i].pos - pos;
	}
	rBody.invI = 12.0/(relMass * (width*width/4 + height*height/4));
	_rigidBodies.push_back(rBody);
}

void Solver::rotateWall(int wallIdx, float angle, Vec2f orig){
	ParticleGroup &pGroup = _wallGroups[wallIdx];
	for (int i=pGroup.startIdx; i<pGroup.endIdx; i++){
		tIndex iGroup = i - pGroup.startIdx;
		_particleData[i].pos = (pGroup.initPos[iGroup] - orig).rotated(angle) + orig;
	}
}

void Solver::rotateGlass(int glassIdx, float angle, Vec2f orig) {
	ParticleGroup &pGroup = _glassGroups[glassIdx];
	for (int i = pGroup.startIdx; i < pGroup.endIdx; i++) {
		tIndex iGroup = i - pGroup.startIdx;
		_particleData[i].pos = (pGroup.initPos[iGroup] - orig).rotated(angle) + orig;
	}
}

Vec2f Solver::getGlassPosition() {
	return _glasscorner;
}

void Solver::setGlassSpeed(Real speedX, Real speedY) {
	_moveGlassSpeedX = speedX;
	_moveGlassSpeedY = speedY;
}

//OPENGL

void Solver::initOpenGL() {
	setupBuffers();
	setupComputeShaderPredictVel();
	setupComputeShaderDensityAlpha();
}

void Solver::cleanupOpenGL() {
	glDeleteBuffers(1, &particleSSBO);
	glDeleteProgram(computeShaderProgramPredictVel);
	glDeleteProgram(computeShaderProgramDensityAlpha);
}

void Solver::setupComputeShaderPredictVel() {

	string path = "src/Simulator/computeShaders/predictVel.comp";
	
	ifstream shaderFile(path);
	if (!shaderFile.is_open()) {
		CORE_ERROR("Failed to open file: {}", path);
		return;
	}

	stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	string shaderCode = shaderStream.str();
	const char* computeShaderSource = shaderCode.c_str();

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &computeShaderSource, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		CORE_ERROR("ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n{}", infoLog);
		glDeleteShader(shader);
	}

	computeShaderProgramPredictVel = glCreateProgram();
	glAttachShader(computeShaderProgramPredictVel, shader);
	glLinkProgram(computeShaderProgramPredictVel);

	glGetProgramiv(computeShaderProgramPredictVel, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(computeShaderProgramPredictVel, 512, NULL, infoLog);
		CORE_ERROR("ERROR::SHADER::COMPUTE::LINKING_FAILED\n{}", infoLog);
		glDeleteShader(shader);
	}

	glDeleteShader(shader);
}

void Solver::setupComputeShaderDensityAlpha() {
	string path = "src/Simulator/computeShaders/computeDensityAlpha.comp";

	ifstream shaderFile(path);
	if (!shaderFile.is_open()) {
		CORE_ERROR("Failed to open file: {}", path);
		return;
	}

	stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	string shaderCode = shaderStream.str();
	const char* computeShaderSource = shaderCode.c_str();

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &computeShaderSource, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		CORE_ERROR("ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n{}", infoLog);
		glDeleteShader(shader);
	}

	computeShaderProgramDensityAlpha = glCreateProgram();
	glAttachShader(computeShaderProgramDensityAlpha, shader);
	glLinkProgram(computeShaderProgramDensityAlpha);

	glGetProgramiv(computeShaderProgramDensityAlpha, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(computeShaderProgramDensityAlpha, 512, NULL, infoLog);
		CORE_ERROR("ERROR::SHADER::COMPUTE::LINKING_FAILED\n{}", infoLog);
		glDeleteShader(shader);
	}

	glDeleteShader(shader);
}

void Solver::setupBuffers() {
	glGenBuffers(1, &particleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
}


void Solver::computeDensityAlphaCS() {

	_neighborsFlat.clear();
	_neighborsCount.clear();
	_neighborOffsets.clear();

	for (int i = 0; i < _neighbors.size(); i++) {
		_neighborOffsets.push_back(_neighborsFlat.size());
		_neighborsFlat.insert(_neighborsFlat.end(), _neighbors[i].begin(), _neighbors[i].end());
		_neighborsCount.push_back(_neighbors[i].size());
	}

	GLuint neighborsBuffer;
	glGenBuffers(1, &neighborsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, neighborsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _neighborsFlat.size() * sizeof(int), _neighborsFlat.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, neighborsBuffer);

	GLuint neighborsCountBuffer;
	glGenBuffers(1, &neighborsCountBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, neighborsCountBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _neighborsCount.size() * sizeof(int), _neighborsCount.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, neighborsCountBuffer);

	GLuint neighborOffsetsBuffer;
	glGenBuffers(1, &neighborOffsetsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, neighborOffsetsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _neighborOffsets.size() * sizeof(int), _neighborOffsets.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, neighborOffsetsBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glBindBuffer in computeDensityAlpha: {}", err);
		return;
	}

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _particleData.size() * sizeof(Particle), _particleData.data());
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glBufferSubData in computeDensityAlpha: {}", err);
		return;
	}

	glUseProgram(computeShaderProgramDensityAlpha);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glUseProgram in computeDensityAlpha: {}", err);
		return;
	}

	GLint m0Location = glGetUniformLocation(computeShaderProgramDensityAlpha, "m0");
	if (m0Location == -1) {
		CORE_ERROR("Failed to get uniform location for m0");
		return;
	}
	glUniform1f(m0Location, _m0);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glUniform1f in computeDensityAlpha: {}", err);
		return;
	}

	GLint supportRadLocation = glGetUniformLocation(computeShaderProgramDensityAlpha, "supportRadius");
	if (supportRadLocation == -1) {
		CORE_ERROR("Failed to get uniform location for supportRadius");
		return;
	}
	glUniform1f(supportRadLocation, _kernel->getSupportRad());
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glUniform1f in computeDensityAlpha: {}", err);
		return;
	}

	GLint particleCountLocation = glGetUniformLocation(computeShaderProgramDensityAlpha, "particleCount");
	if (particleCountLocation == -1) {
		CORE_ERROR("Failed to get uniform location for particleCount");
		return;
	}
	glUniform1i(particleCountLocation, _particleCount);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glUniform1i in computeDensityAlpha: {}", err);
		return;
	}

	GLint lookup_fLocation = glGetUniformLocation(computeShaderProgramDensityAlpha, "lookup_f");
	if (lookup_fLocation == -1) {
		CORE_ERROR("Failed to get uniform location for lookup_f");
		return;
	}
	glUniform1i(lookup_fLocation, 3); // texture unit 3

	GLint lookup_dfLocation = glGetUniformLocation(computeShaderProgramDensityAlpha, "lookup_df");
	if (lookup_dfLocation == -1) {
		CORE_ERROR("Failed to get uniform location for lookup_df");
		return;
	}
	glUniform1i(lookup_dfLocation, 4); // texture unit 4

	GLuint lookup_fTexture;
	glGenTextures(1, &lookup_fTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_1D, lookup_fTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, LOOKUP_SIZE, 0, GL_RED, GL_FLOAT, _kernel->getLookupF());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint lookup_dfTexture;
	glGenTextures(1, &lookup_dfTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_1D, lookup_dfTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, LOOKUP_SIZE, 0, GL_RED, GL_FLOAT, _kernel->getLookupDF());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint numGroups = (_particleData.size() + 127) / 128;
	glDispatchCompute(numGroups, 1, 1);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glDispatchCompute in computeDensityAlpha: {}", err);
		return;
	}

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glMemoryBarrier in computeDensityAlpha: {}", err);
		return;
	}

	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _particleData.size() * sizeof(Particle), _particleData.data());
	err = glGetError();
	if (err != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glGetBufferSubData in computeDensityAlpha: {}", err);
		return;
	}

	glDeleteTextures(1, &lookup_fTexture);
	glDeleteTextures(1, &lookup_dfTexture);
	glDeleteBuffers(1, &neighborsBuffer);
	glDeleteBuffers(1, &neighborsCountBuffer);
	glDeleteBuffers(1, &neighborOffsetsBuffer);

}

void Solver::predictVelCS(const Real dt) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _particleData.size() * sizeof(Particle), _particleData.data());
	glUseProgram(computeShaderProgramPredictVel);
	GLint dtLocation = glGetUniformLocation(computeShaderProgramPredictVel, "dt");
	glUniform1f(dtLocation, dt);
	GLuint numGroups = (_particleData.size() + 127) / 128;
	glDispatchCompute(numGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _particleData.size() * sizeof(Particle), _particleData.data());
}

void Solver::resizeSSBO() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _particleData.size() * sizeof(Particle), _particleData.data(), GL_DYNAMIC_COPY);
}