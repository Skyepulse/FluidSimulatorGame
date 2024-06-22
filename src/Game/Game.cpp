#include "Game.h"
#include <algorithm>

Game::Game() : LevelLayer("Game Layer", Bound(glm::vec2(0), glm::vec2(36, 72)))
{
}

Game::~Game()
{
}

void Game::OnAttach()
{
	circleLiquid = std::make_shared<Circle>();
	circleGlass = std::make_shared<Circle>();


	float circleRadius = 0.1f;
	circleLiquid->Transform->Scale2D(circleRadius);
	circleGlass->Transform->Scale2D(circleRadius);

	//circleWalls->SetColor(glm::vec3(0.6f));
	circleLiquid->SetColor(glm::vec3(0.2f, 0.3f, 1.0f));
	circleGlass->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));

	Real resX = 36.0f;
	Real resY = 50.0f;
	//solver.initSimulation(resX, resY);

	//Draw level
	//solver.drawWalls(resX, resY);
	m_Solver.drawAngleLineWall(Vec2f(0, 7 * resY/10), 45, -30, 1);
	m_Solver.drawAngleLineWall(Vec2f(20, 3 * resY / 10), 45, 30, 1);
	int width = 10;
	int height = 10;
	m_Solver.drawWinningGlass(width, height, Vec2f(1, 1));

	m_Solver.addRigidBody(Vec2f(2*width, 5), width, height, 100);

	m_Solver.setSpawnPosition(Vec2f(4, resY - 4));

	m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10, 0, ViscosityType::FLUID);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();
}

void Game::OnDetach()
{
}

void Game::UpdateGame()
{
	Vec2f velVec = Vec2f(0, 0);
	if (moveGlassLeft) velVec.x -= _moveGlassSpeedX;
	if (moveGlassRight) velVec.x += _moveGlassSpeedX;
	if (moveGlassUp) velVec.y += _moveGlassSpeedY;
	if (moveGlassDown) velVec.y -= _moveGlassSpeedY;
	m_Solver.moveGlass(winningGlassIndex, velVec, true);
}

bool Game::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT
			&& keypressed.GetKey() != CORE_KEY_UP && keypressed.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) m_Solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius), ViscosityType::FLUID);
		if (keypressed.GetKey() == CORE_KEY_LEFT) moveGlassLeft = true;
		if (keypressed.GetKey() == CORE_KEY_RIGHT) moveGlassRight = true;

		return true;
	} else if(e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT
			&& keyreleased.GetKey() != CORE_KEY_UP && keyreleased.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) moveGlassLeft = false;
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) moveGlassRight = false;

		return true;
	}
	
	return false;
}

Vec2f Game::getRandomPointInCircle(const Vec2f &center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}