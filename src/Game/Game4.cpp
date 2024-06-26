#include "Game4.h"
#include <algorithm>

Game4::Game4() : LevelLayer("Game4 Layer", Bound(glm::vec2(36.0, 50.0)))
{
}

Game4::~Game4()
{
}

void Game4::OnAttach()
{
	Real resX = 36.0f;
	Real resY = 50.0f;

	//Draw level
	m_Solver.drawAngleLineWall(Vec2f(0.0f, 7.0f * resY / 10.0f), 30, -30, 1);
	m_Solver.drawAngleLineWall(Vec2f(23.0f, 5.5f * resY / 10.0f), 30, 30, 1);
	m_Solver.drawAngleLineWall(Vec2f(7, 3.0f * resY / 10.0f), 43, 0, 1);
	int width = 10;
	int height = 10;

	winningGlassIndex = 0;
	m_Solver.drawWinningGlass(width, height, Vec2f(16, 1));

	m_Solver.setSpawnPosition(Vec2f(4, resY - 4));

	m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 5, 0, ViscosityType::FLUID);
	m_Solver.spawnLiquidRectangle(Vec2f(resX - 12, resY - 15), 10, 5, 0, ViscosityType::VISCOUS);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();

	GlassMinX = 1;
	GlassMaxX = -width / 2.0f - 1;

	Application::Get()->GetUI()->setHintMessage("What about two viscosities at once?");
}

void Game4::OnDetach()
{
}

void Game4::UpdateGame()
{
	Vec2f velVec = Vec2f(0, 0);
	Vec2f pos = m_Solver.getGlassPosition();
	if (moveGlassLeft && pos.x > GlassMinX) velVec.x -= _moveGlassSpeedX;
	if (moveGlassRight && pos.x < resX + GlassMaxX) velVec.x += _moveGlassSpeedX;
	if (moveGlassUp) velVec.y += _moveGlassSpeedY;
	if (moveGlassDown) velVec.y -= _moveGlassSpeedY;
	m_Solver.moveGlass(winningGlassIndex, velVec, true);
}

bool Game4::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT
			&& keypressed.GetKey() != CORE_KEY_UP && keypressed.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keypressed.GetKey() == CORE_KEY_LEFT) moveGlassLeft = true;
		if (keypressed.GetKey() == CORE_KEY_RIGHT) moveGlassRight = true;
		return true;
	}
	else if (e.GetEventType() == EventType::KeyReleased) {
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

Vec2f Game4::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}