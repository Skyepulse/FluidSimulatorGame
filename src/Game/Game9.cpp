#include "Game9.h"
#include <algorithm>

Game9::Game9() : LevelLayer("Game Layer", Bound(glm::vec2(36.0f, 50.0f)))
{
}

Game9::~Game9()
{
}

void Game9::OnAttach()
{
	Real resX = 36.0f;
	Real resY = 50.0f;
	_resX = resX;
	_resY = resY;

	//Draw level
	m_Solver.drawAngleLineWall(Vec2f(26, 0), 60, 90);

	int width = 10;
	int height = 10;
	m_Solver.drawWinningGlass(width, height, Vec2f(1, 1));

	m_Solver.addRigidBody(Vec2f(28, 1), width, height, 10);
	m_Solver.setDefaultDt(0.01f);

	m_Solver.setSpawnPosition(Vec2f(resX-4, resY - 4));

	pipe->Transform->Translate2D(glm::vec2(resX-4, resY - 4));

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();

	_glassRange = resY / 8 * 0.8;

	startTime = maxTime;

	Application::Get()->GetUI()->setHintMessage("Press P to release water !");
}

void Game9::OnDetach()
{
}

void Game9::UpdateGame()
{
	Renderer::DrawShape(pipe);
}

bool Game9::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() == CORE_KEY_LEFT) pipe->Transform->Translate2D(glm::vec2(-1, 0));
		if (keypressed.GetKey() == CORE_KEY_RIGHT) pipe->Transform->Translate2D(glm::vec2(1, 0));

		if (keypressed.GetKey() == CORE_KEY_P) {
			m_Solver.spawnParticle(particleSpawnPosition, particleSpawnRadius, ViscosityType::FLUID, Vec2f(0, -10));
		}

		return true;
	}
	return false;
}

Vec2f Game9::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}