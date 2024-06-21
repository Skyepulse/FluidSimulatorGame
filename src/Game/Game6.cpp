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

	solver.initSimulation(resX, resY);

	//Draw level
	//solver.drawWalls(resX, resY);
	//solver.drawAngleLineWall(Vec2f(0, 0), resX*2, 0, 1);
	solver.drawAngleLineWall(Vec2f(1, 1), resY * 2 - 4, 90, 1);
	solver.drawAngleLineWall(Vec2f(resX, 1), resY * 2 - 4, 90, 1);
	int width = 18;
	int height = 18;
	glassWidth = width;
	glassHeight = height;
	solver.setSpawnPosition(Vec2f(resX - 4, resY - 4));

	solver.drawWinningGlass(width, height, Vec2f(resX - width, 10));
	solver.activateInfiniteWalls();
	solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10, 0, ViscosityType::FLUID);

	solver.init();

	winningGlassParticles = solver.getWinningGlass();
	particleSpawnPosition = solver.getSpawnPosition();
}

void Game6::OnDetach()
{
}

void Game6::Update()
{
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
			break;
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

	Vec2f velVec = Vec2f(0, 0);
	if (_moveGlassLeft) velVec.x -= _glassSpeedX;
	if (_moveGlassRight) velVec.x += _glassSpeedX;
	solver.moveGlass(winningGlassIndex, velVec, true);

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

		if (keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT
			&& keypressed.GetKey() != CORE_KEY_UP && keypressed.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keypressed.GetKey() == CORE_KEY_LEFT) _moveGlassLeft = true;
		if (keypressed.GetKey() == CORE_KEY_RIGHT) _moveGlassRight = true;
		return true;
	}
	else if (e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT
			&& keyreleased.GetKey() != CORE_KEY_UP && keyreleased.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) _moveGlassLeft = false;
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) _moveGlassRight = false;

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