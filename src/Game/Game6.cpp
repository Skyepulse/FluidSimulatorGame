#include "Game6.h"
#include <algorithm>

Game6::Game6() : LevelLayer("Game Layer", Bound(glm::vec2(36.0, 50.0)))
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

	//Draw level
	//m_Solver.drawAngleLineWall(Vec2f(0, 0), resX*2, 0, 1);
	m_Solver.drawAngleLineWall(Vec2f(1, 1), resY * 2 - 4, 90, 1);
	m_Solver.drawAngleLineWall(Vec2f(resX, 1), resY * 2 - 4, 90, 1);
	int width = 18;
	int height = 18;
	glassWidth = width;
	glassHeight = height;
	m_Solver.setSpawnPosition(Vec2f(resX - 4, resY - 4));

	m_Solver.drawWinningGlass(width, height, Vec2f(resX - width, 10));
	m_Solver.activateInfiniteWalls();
	m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10, 0, ViscosityType::FLUID);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();
}

void Game6::OnDetach()
{
}

void Game6::UpdateGame()
{
	Vec2f velVec = Vec2f(0, 0);
	if (_moveGlassLeft) velVec.x -= _glassSpeedX;
	if (_moveGlassRight) velVec.x += _glassSpeedX;
	m_Solver.moveGlass(winningGlassIndex, velVec, true);
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