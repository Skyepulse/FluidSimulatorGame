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

	m_Solver.addRigidBody(Vec2f(2*width, 5), width, height, 10);

	m_Solver.setSpawnPosition(Vec2f(4, resY - 4));

	m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10);

	m_Solver.setGlassSpeed(4.0f, 0.0f);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();
}

void Game::OnDetach()
{
}

void Game::UpdateGame()
{
	Real _dt = 0.0f;
	if (m_State != GameState::PAUSED) _dt = m_Solver.update();
	if (m_State == GameState::RUNNING) maxTime -= _dt;
	if (maxTime < 0.0) {
		maxTime = 0.0;
		m_State = GameState::LOSE;
	}

	vector<Vec2f> wallsPositions;
	vector<Vec2f> liquidPositions;
	vector<Vec2f> glassPositions;

	for (size_t i = 0; i < m_Particles.size(); i++)
	{
		if (m_Particles[i].type == 1)
			wallsPositions.push_back(m_Particles[i].pos);
		else if(m_Particles[i].type == 0)
			liquidPositions.push_back(m_Particles[i].pos);
		else
			glassPositions.push_back(m_Particles[i].pos);

	}

	Renderer::DrawShapeDuplicate(m_WallParticle, wallsPositions);
	Renderer::DrawShapeDuplicate(circleLiquid, liquidPositions);
	Renderer::DrawShapeDuplicate(circleGlass, glassPositions);

	int particlesInGlass = m_Solver.getParticlesInGlass();
	if (particlesInGlass >= winningGlassParticles)
	{
		circleGlass->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		m_State = GameState::WIN;
	}
}

bool Game::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) m_Solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius));
		if (keypressed.GetKey() == CORE_KEY_LEFT) m_Solver.moveGlassLeft(true);
		if (keypressed.GetKey() == CORE_KEY_RIGHT) m_Solver.moveGlassRight(true);


		return true;
	} else if(e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) m_Solver.moveGlassLeft(false);
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) m_Solver.moveGlassRight(false);

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