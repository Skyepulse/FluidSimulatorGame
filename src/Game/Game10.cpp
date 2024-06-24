#include "Game10.h"
#include <algorithm>

Game10::Game10() : LevelLayer("Game Layer", Bound(glm::vec2(60.0, 80.0)), false)
{
}

Game10::~Game10()
{
}

void Game10::OnAttach()
{
	Real resX = 60.0f;
	Real resY = 80.0f;
	_resX = resX;
	_resY = resY;

	//Draw level
	m_Solver.drawAngleLineWall(Vec2f(0, resY - 25), 20, -75);
	m_Solver.drawAngleLineWall(Vec2f(3, resY - 35), 15, -30);
	m_Solver.drawAngleLineWall(Vec2f(10, resY - 39), 10, 0);
	m_Solver.drawAngleLineWall(Vec2f(resX - 11, 30), 22, 0);

	int width = 18;
	int height = 18;
	glassWidth = width;
	glassHeight = height;

	m_Solver.drawWinningGlass(width , height , Vec2f(resX - 10, 9));
	m_Solver.activateInfiniteWalls();
	m_Solver.setSpawnPosition(Vec2f(4, resY - 4));
	pipe->Transform->Translate2D(glm::vec2(4, resY - 4));

	m_Solver.spawnLiquidRectangle(Vec2f(20, resY - 20), 2, 10, 0, SLIGHTLY);
	m_Solver.spawnLiquidRectangle(Vec2f(30, resY - 10), 2, 10, 0, MEDIUM);
	m_Solver.spawnLiquidRectangle(Vec2f(40, resY), 2, 10, 0, VISCOUS);

	m_Solver.setViscosityForWin(FLUID);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();

	Application::Get()->GetUI()->setHintMessage("Press P to release water !");
}

void Game10::OnDetach()
{
}

void Game10::UpdateGame()
{
	Vec2f velVec = Vec2f(0, 0);
	if (_moveGlassLeft) velVec.x -= _glassSpeedX;
	if (_moveGlassRight) velVec.x += _glassSpeedX;
	m_Solver.moveGlass(winningGlassIndex, velVec, true);

	Renderer::DrawShape(pipe);
}

bool Game10::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) {
			m_Solver.spawnParticle(particleSpawnPosition, particleSpawnRadius, ViscosityType::FLUID, Vec2f(0, -10));
		}

		return true;
	}
	return false;
}

Vec2f Game10::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}