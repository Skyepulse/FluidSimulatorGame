#include "Game.h"
#include <algorithm>

#include "../Core/Rendering/Renderer.h"
#include "../Core/Event/ApplicationEvent.h"
#include "../Core/Event/KeyCode.h"

Game::Game()
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

	solver.initSimulation(36.0f, 72.0f);
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
  if(e.GetEventType() != EventType::KeyPressed)
    return false;
  
  KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

  if(keypressed.GetKey() != CORE_KEY_P)
    return false;
  
  CORE_DEBUG("Particle Spawned")
  solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius));
  return true;
}

Vec2f Game::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}