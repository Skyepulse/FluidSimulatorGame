#include "Game6.h"
#include <algorithm>

Game6::Game6() : LevelLayer("Game Layer")
{
}

Game6::~Game6()
{
}

void Game6::OnAttach()
{
	circleWalls = std::make_shared<Circle>();
	circleLiquid = std::make_shared<Circle>();
	circleGlass = std::make_shared<Circle>();

	float circleRadius = 0.5f;
	circleWalls->Transform->Scale2D(circleRadius);
	circleLiquid->Transform->Scale2D(circleRadius);
	circleGlass->Transform->Scale2D(circleRadius);

	circleWalls->SetColor(glm::vec3(0.6f));
	circleLiquid->SetColor(glm::vec3(0.2f, 0.3f, 1.0f));
	circleGlass->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));

	Real resX = 36.0f;
	Real resY = 50.0f;
	_resX = resX;
	_resY = resY;

	_maxParticles = 100;
	solver.setMaxParticles(_maxParticles);


	solver.initSimulation(resX, resY);

	//Draw level
	//solver.drawWalls(resX, resY);
	//solver.drawAngleLineWall(Vec2f(0, 0), resX*2, 0, 1);
	solver.drawAngleLineWall(Vec2f(1, 1), resY * 2 - 4, 90, 1);
	solver.drawAngleLineWall(Vec2f(resX, 1), resY * 2 - 4, 90, 1);
	int width = 16;
	int height = 10;
	glassWidth = width;
	glassHeight = height;
	solver.setSpawnPosition(Vec2f(resX - 4, resY - 4));
	solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10, 0, ViscosityType::FLUID);

	//solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10);

	solver.init();

	winningGlassParticles = solver.getWinningGlass();
	particleSpawnPosition = solver.getSpawnPosition();

	_glassMoveUp = true;
}

void Game6::OnDetach()
{
}

void Game6::Update()
{
	/*
	Vec2f glassPosition = solver.getGlassPosition();
	Vec2f velocityVector = Vec2f(0, 0);
	if (glassPosition.y >= _resY - glassHeight)
	{
		_glassMoveUp = false;
		_glassMoveDown = true;
	}
	else if (glassPosition.y <= 1)
	{
		_glassMoveDown = false;
		_glassMoveUp = true;
	}
	if (_glassMoveUp)
		velocityVector = Vec2f(0, _glassSpeedY);
	else if (_glassMoveDown)
		velocityVector = Vec2f(0, -_glassSpeedY);
	solver.moveGlass(glassIndex, velocityVector, true);
	*/


	// ----------- HANDLE FRAMERATE ----------
	Real _dt = 0.0f;
	double currentTime = Time::GetSeconds();
	double frameTime = currentTime - previousTime;
	previousTime = currentTime;
	accumulator += frameTime;

	if (!SHOULD_FPSCAP) accumulator = 0.0;

	while (accumulator >= MIN_FRAME_TIME) {
		if (state != GameState::PAUSED) {
			double step = solver.update();
			accumulator -= step;
			_dt += step;
		}
		else {
			accumulator = 0.0;
		}
	}
	if (state == GameState::RUNNING) maxTime -= _dt;
	if (maxTime < 0.0) {
		maxTime = 0.0;
		state = GameState::LOSE;
	}

	if (accumulator > MIN_FRAME_TIME) return;

	// ----------- END HANDLE FRAMERATE ----------


	vector<Particle> particleManager = solver.getParticleManager();

	vector<Vec2f> wallsPositions;
	vector<Vec2f> liquidPositions;
	vector<Vec2f> glassPositions;

	for (size_t i = 0; i < particleManager.size(); i++)
	{
		if (particleManager[i].type == 1)
			wallsPositions.push_back(particleManager[i].pos);
		else if (particleManager[i].type == 0)
			liquidPositions.push_back(particleManager[i].pos);
		else
			glassPositions.push_back(particleManager[i].pos);

	}

	Renderer::DrawShapeDuplicate(circleWalls, wallsPositions);
	Renderer::DrawShapeDuplicate(circleLiquid, liquidPositions);
	Renderer::DrawShapeDuplicate(circleGlass, glassPositions);

	int particlesInGlass = solver.getParticlesInGlass();
	if (particlesInGlass >= winningGlassParticles)
	{
		circleGlass->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		state = GameState::WIN;
	}

}

bool Game6::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) {
			solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius), ViscosityType::FLUID);
		}

		return true;
	}
	return false;
}

Vec2f Game6::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}