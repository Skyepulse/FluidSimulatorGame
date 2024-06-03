#include "Game.h"
#include <algorithm>

Game::Game() : Layer("Game Layer")
{
}

Game::~Game()
{
}

void Game::Init()
{
	circleWalls  = std::make_shared<Circle>();
	circleLiquid = std::make_shared<Circle>();
	circleGlass  = std::make_shared<Circle>();

	float circleRadius = 0.5f;
	circleWalls->Transform->Scale2D(circleRadius);
	circleLiquid->Transform->Scale2D(circleRadius);
	circleGlass->Transform->Scale2D(circleRadius);

	circleWalls->SetColor(glm::vec3(0.6f));
	circleLiquid->SetColor(glm::vec3(0.2f, 0.3f, 1.0f));
	circleGlass->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));

	Real resX = 36.0f;
	Real resY = 72.0f;
	solver.initSimulation(resX, resY);

	//Draw level
	solver.drawWalls(resX, resY);
	solver.drawAngleLineWall(Vec2f(0, 7 * resY / 10), 45, -30);
	solver.drawAngleLineWall(Vec2f(resX, 4 * resY / 10), 45, -160);
	int width = 10;
	int height = 10;
	solver.drawWinningGlass(width, height, Vec2f(1, 1));
	solver.setSpawnPosition(Vec2f(4, resY - 4));

	solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10);

	solver.init();

	winningGlassParticles = solver.getWinningGlass();
	particleSpawnPosition = solver.getSpawnPosition();
}

void Game::Update()
{
	solver.update();
	ParticleManager particleManager = solver.getParticleManager();
	vector<Vec2f> positions = particleManager.pos;
	vector<int> types = particleManager.type;

	vector<Vec2f> wallsPositions;
	vector<Vec2f> liquidPositions;
	vector<Vec2f> glassPositions;

	for (size_t i = 0; i < positions.size(); i++)
	{
		if (types[i] == 1)
			wallsPositions.push_back(positions[i]);
		else if(types[i] == 0)
			liquidPositions.push_back(positions[i]);
		else
			glassPositions.push_back(positions[i]);

	}

	Renderer::DrawShapeDuplicate(circleWalls, wallsPositions);
	Renderer::DrawShapeDuplicate(circleLiquid, liquidPositions);
	Renderer::DrawShapeDuplicate(circleGlass, glassPositions);

	int particlesInGlass = solver.getParticlesInGlass();
	if (particlesInGlass >= winningGlassParticles)
	{
		circleGlass->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	}

}

bool Game::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius));
		if (keypressed.GetKey() == CORE_KEY_LEFT) solver.moveGlassLeft(true);
		if (keypressed.GetKey() == CORE_KEY_RIGHT) solver.moveGlassRight(true);


		return true;
	} else if(e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) solver.moveGlassLeft(false);
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) solver.moveGlassRight(false);

		return true;
	}
	
	return false;
}

void Game::OnAttach()
{
}

void Game::OnDetach()
{
}

Vec2f Game::getRandomPointInCircle(const Vec2f &center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}