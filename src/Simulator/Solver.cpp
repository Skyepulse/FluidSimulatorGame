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

	_particleData.clear();

	_neighbors.clear();

	_particlesInGrid.clear();
	_particlesInGrid.resize(resX * resY);

	_particleCount = 0;
	_immovableParticleCount = 0;
	_immovableGlassParticleCount = 0;

	Real sr = _kernel->getSupportRad();
}

void Solver::addParticle(const Vec2f& pos, const int type, const Vec2f& vel, const Vec2f& acc, const Real press, const Real density, const Real alpha)
{
	Particle p;
	p.pos = pos;
	p.vel = vel;
	p.acc = acc;
	p.press = press;
	p.density = density;
	p.type = type;
	p.alpha = alpha;
	p.isInGlass = false;

	_particleData.push_back(p);
	_neighbors.push_back(vector<tIndex>());

	if(type == 1) _immovableParticleCount++;
	if(type == 2) _immovableGlassParticleCount++;
	_particleCount++;
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
	return p;
}

void Solver::init() {
	buildNeighbors();
	computeDensity();
	computeAlpha();
	//CORE_DEBUG("///////////////////////////////////init done/////////////////////////////////");
}

void Solver::update() {
	//CORE_DEBUG("///////////////////////////////////UPDATE/////////////////////////////////");	
    //CORE_DEBUG("Particle count in glass: {}", _particlesInGlass);
	computeNPforces();
	//CORE_DEBUG("NP forces: {}", Time::GetDeltaTime());
	adaptDt();
	//CORE_DEBUG("Adapt dt: {}", Time::GetDeltaTime());
	predictVelCS(_dt);
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
		int x = _particleData[i].pos.x / R;
		int y = _particleData[i].pos.y / R;
		tIndex idx = idx1d(x, y);
		if (x >= 0 && y >= 0 && x < _resX && y < _resY) {
			_particlesInGrid[idx].push_back(i);
		}
	}

	_neighbors.clear();
	_neighbors.resize(_particleCount);

	// Then we build the neighbors
	for (int i = 0; i < _particleCount; i++) {
		int x = _particleData[i].pos.x / R;
		int y = _particleData[i].pos.y / R;
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				tIndex idx = idx1d(x + dx, y + dy);
				if (x + dx >= 0 && y + dy >= 0 && x + dx < _resX && y + dy < _resY) {
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

void Solver::computeDensity() {
	for (int i = 0; i < _particleCount; i++) {
		_particleData[i].density = 0;
		for (int j = 0; j < _neighbors[i].size(); j++) {
			tIndex p = _neighbors[i][j];
			_particleData[i].density += _m0 * _kernel->W(_particleData[i].pos - _particleData[p].pos);
		}
	}
}

void Solver::computeAlpha() {
	for (int i = 0; i < _particleCount; i++) {
		Vec2f a = Vec2f(0e0);
		Real b = 0e0;
		for (int j = 0; j < _neighbors[i].size(); j++) {
			tIndex p = _neighbors[i][j];
			Vec2f factor = _m0 * _kernel->gradW(_particleData[i].pos - _particleData[p].pos);
			b += _particleData[p].type == 1 ? 0 : factor.lengthSquare();
			a += factor;
		}

		_particleData[i].alpha = 1.0/max(b + a.lengthSquare(), 1.0e-6f);
	}
}

void Solver::computeNPforces() {
	for (int i = 0; i < _particleCount; i++) {
		if(_particleData[i].type == 1 || _particleData[i].type == 2) continue;
		// gravity
		_particleData[i].acc = _g;

		for (int j=0; j<_neighbors[i].size(); j++) {
			// suppose all masses are equal
			tIndex p = _neighbors[i][j];

			// viscosity force
			Vec2f x = (_particleData[i].pos - _particleData[p].pos);
            Vec2f u = (_particleData[i].vel - _particleData[p].vel);
            _particleData[i].acc += 2.0f * _nu * _m0 / _particleData[p].density * u * x.dotProduct(_kernel->gradW(_particleData[i].pos - _particleData[p].pos)) / (x.dotProduct(x) + 0.01f * _h * _h);
		}
	}
}

void Solver::predictVel(const Real dt){
	for (int i = 0; i < _particleCount; i++) {
		if (_particleData[i].type == 0)
			_particleData[i].vel += dt * (_particleData[i].acc);
	}
}

void Solver::adaptDt() {
	Real maxVel2 = 0e0;
	for (int i = 0; i < _particleCount; i++) {
		maxVel2 = max(maxVel2, _particleData[i].vel.lengthSquare());
	}
	Real maxVel = sqrt(maxVel2);
	if(maxVel == 0e0) _dt = DEFAULT_DT;
	else _dt = 0.6*_h/maxVel;
	_dt = min(_dt, DEFAULT_DT);
}

void Solver::updatePos(const Real dt) {
	for (int i = 0; i < _particleCount; i++) {
		if(_particleData[i].type == 1) continue;
		if (_particleData[i].type == 2) {
			if (this->_moveGlassRight) {
				_particleData[i].pos += dt * _moveGlassSpeedX * Vec2f(1.0f, 0.0f);
			}
			if (this->_moveGlassLeft) {
				_particleData[i].pos += dt * _moveGlassSpeedX * Vec2f(-1.0f, 0.0f);
			}
			if (this->_moveGlassUp) {
				_particleData[i].pos += dt * _moveGlassSpeedY * Vec2f(0.0f, 1.0f);
			}
			if (this->_moveGlassDown) {
				_particleData[i].pos += dt * _moveGlassSpeedY * Vec2f(0.0f, -1.0f);
			}
			continue;
		}
		_particleData[i].pos += dt * _particleData[i].vel;

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
}

void Solver::correctDivergenceError(const Real dt){
	vector<Real> dp(_particleCount);
	Real dpAvg = (Real)INT_MAX;
	Real eta = 10.0f;
	Real dtInv = (Real)1.0f / dt;

	int iter = 0;

	for (int i=0; i<_particleCount; i++){
		_particleData[i].alpha *= dtInv;
	}

	while ((abs(dpAvg) > eta || iter < 1) && iter < 10){
		dpAvg = 0.0f;
		for (tIndex i=0; i<_particleCount; i++){
			dp[i] = 0e0f;
			for (int p = 0; p < _neighbors[i].size(); p++) {
				tIndex j = _neighbors[i][p];
				dp[i] += (_particleData[i].vel - _particleData[j].vel).dotProduct(_kernel->gradW(_particleData[i].pos - _particleData[j].pos));
			}
			if (_particleData[i].type == 0) dpAvg += _m0 * dp[i];
		}
		dpAvg /= _particleCount;

		for (tIndex i=0; i<_particleCount; i++){
			if (_particleData[i].type == 1) continue;
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
		_particleData[i].alpha *= dt2Inv;
	}

	while ((iter < 2 || abs(densAvg - _d0) > eta) && iter < 10){
		Time::GetDeltaTime();
		densAvg = 0.0f;
		for (tIndex i=0; i<_particleCount; i++){
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
	this->_winningGlass = (width - 2) * (height - 1) / 2;
}

void Solver::spawnParticle(Vec2f position) {
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
	addParticle(position, 0);
}

void Solver::setSpawnPosition(Vec2f position) {
	_spawnPosition = this->_kernel->getSupportRad()*position;
}

void Solver::spawnLiquidRectangle(Vec2f position, int width, int height, int type) {
	Real sr = _kernel->getSupportRad();
	for (int i = position.x; i < position.x + width; i++) {
		for (int j = position.y; j < position.y + height; j++) {
			addParticle(sr * Vec2f(i + 0.25, j + 0.25));
			addParticle(sr * Vec2f(i + 0.75, j + 0.25));
			addParticle(sr * Vec2f(i + 0.25, j + 0.75));
			addParticle(sr * Vec2f(i + 0.75, j + 0.75));
		}
	}
}

//OPENGL

void Solver::initOpenGL() {
	setupComputeShader();
	setupBuffers();
}

void Solver::cleanupOpenGL() {
	glDeleteBuffers(1, &particleSSBO);
	glDeleteProgram(computeShaderProgram);
}

void Solver::setupComputeShader() {
	const char* computeShaderSource = R"(
    #version 430
    layout(local_size_x = 128) in;

    struct Particle {
        vec2 pos;
        vec2 vel;
        vec2 acc;
        float press;
        float density;
        float alpha;
        int type;
        bool isInGlass;
    };

    layout(std430, binding = 0) buffer ParticleBuffer {
        Particle particles[];
    };

    uniform float dt;

    void main() {
        uint idx = gl_GlobalInvocationID.x;
        if (particles[idx].type == 0) {
            particles[idx].vel += dt * particles[idx].acc;
        }
    }
    )";

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &computeShaderSource, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		CORE_ERROR("ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n{}", infoLog);
	}

	computeShaderProgram = glCreateProgram();
	glAttachShader(computeShaderProgram, shader);
	glLinkProgram(computeShaderProgram);

	glGetProgramiv(computeShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(computeShaderProgram, 512, NULL, infoLog);
		CORE_ERROR("ERROR::SHADER::COMPUTE::LINKING_FAILED\n{}", infoLog);
	}

	glDeleteShader(shader);
}

void Solver::setupBuffers() {
	glGenBuffers(1, &particleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _particleData.size() * sizeof(Particle), _particleData.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
}

void Solver::predictVelCS(const Real dt) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _particleData.size() * sizeof(Particle), _particleData.data());

	glUseProgram(computeShaderProgram);

	GLint dtLocation = glGetUniformLocation(computeShaderProgram, "dt");
	if (dtLocation == -1) {
		CORE_ERROR("Failed to get uniform location for dt");
		return;
	}
	glUniform1f(dtLocation, dt);

	GLuint numGroups = (_particleData.size() + 127) / 128;
	cout << "numGroups: " << numGroups << endl;
	cout << "particleData size: " << _particleData.size() << endl;
	glDispatchCompute(numGroups, 1, 1);

	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glDispatchCompute: {}", err);
		return;
	}

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	if ((err = glGetError()) != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glMemoryBarrier: {}", err);
		return;
	}

	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _particleData.size() * sizeof(Particle), _particleData.data());

	if ((err = glGetError()) != GL_NO_ERROR) {
		CORE_ERROR("OpenGL error after glGetBufferSubData: {}", err);
		return;
	}
}
	