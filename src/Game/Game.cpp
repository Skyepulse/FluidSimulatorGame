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

	float circleRadius = 0.2f;
	circleWalls->Transform->Scale2D(circleRadius);
	circleLiquid->Transform->Scale2D(circleRadius);
	circleWalls->SetColor(glm::vec3(0.6f));
	circleLiquid->SetColor(glm::vec3(0.2f, 0.3f, 0.8f));

	solver.initSimulation(36.0f, 72.0f);
}

void Game::Update()
{
	solver.update(0.005f);
	ParticleManager particleManager = solver.getParticleManager();
	vector<Vec2f> positions = particleManager.pos;
	vector<int> types = particleManager.type;

	vector<Vec2f> wallsPositions;
	vector<Vec2f> liquidPositions;

	for (size_t i = 0; i < positions.size(); i++)
	{
		if (types[i] == 1)
			wallsPositions.push_back(positions[i]);
		else
			liquidPositions.push_back(positions[i]);
	}

	Renderer::DrawShapeDuplicate(circleWalls, wallsPositions);
	Renderer::DrawShapeDuplicate(circleLiquid, liquidPositions);
}

bool Game::OnEvent(Event& e)
{
  if(e.GetEventType() != EventType::KeyPressed)
    return false;
  
  KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

  if(keypressed.GetKey() != CORE_KEY_P)
    return false;
  
  CORE_DEBUG("Particle Spawned")
  // TODO : Spawn particles

  return true;
}