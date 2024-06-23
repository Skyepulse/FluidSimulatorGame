#include "Game2.h"
#include <algorithm>

Game2::Game2() : LevelLayer("Game Layer", Bound(glm::vec2(36.0f, 50.0f)), false)
{
}

Game2::~Game2()
{
}

void Game2::OnAttach()
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
	m_Solver.setMaxParticles(_maxParticles);

	//Draw level
	//m_Solver.drawWalls(resX, resY);
	//m_Solver.drawAngleLineWall(Vec2f(0, 0), resX*2, 0, 1);
	m_Solver.drawAngleLineWall(Vec2f(0, resY-1), resX*2, 0, 1);

	m_Solver.drawAngleLineWall(Vec2f(1, 1), resY*2 - 4, 90, 1);
	m_Solver.drawAngleLineWall(Vec2f(resX, 1), resY*2 - 4, 90, 1);
	m_Solver.drawAngleLineWall(Vec2f(20, 5 * resY / 10), 45, 30, 1);
	int width = 16;
	int height = 10;
	glassWidth = width;
	glassHeight = height;

	m_Solver.drawWinningGlass(width, height, Vec2f(6, 1));
	m_Solver.setSpawnPosition(Vec2f(resX-4, resY - 4));

	pipe->Transform->Translate2D(glm::vec2(resX-4, resY - 4));

	//m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();

	_glassRange = resY / 8 * 0.8;

	startTime = maxTime;

	Application::Get()->GetUI()->setHintMessage("Press P to release water !");
}

void Game2::OnDetach()
{
}

void Game2::UpdateGame()
{
	Vec2f velocityVector = Vec2f(0, _glassRange * glm::sin((startTime - maxTime) * M_PI / _glassPeriod));
	m_Solver.moveGlass(glassIndex, velocityVector, true);

	Renderer::DrawShape(pipe);
}

bool Game2::OnEvent(Event& e)
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

Vec2f Game2::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}