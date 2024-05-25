#include "Game.h"

#include "../Core/Rendering/Renderer.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init()
{
	circle  = std::make_shared<Circle>();
	float circleRadius = 0.2f;
	circle->Transform->Scale2D(circleRadius);
	circle->SetColor(glm::vec3(1.0f));

	solver.initSimulation(96.0f, 72.0f);
}

void Game::Update()
{
	solver.update(0.005f);
	ParticleManager particleManager = solver.getParticleManager();

	Renderer::DrawShapeDuplicate(circle, particleManager.pos);
}

bool Game::OnEvent(Event& e)
{
  // Return event handled or not ? 
  // If no, continue processing event
  // If yes, event is destroyed

  return false;
}