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

	solver.initSimulation(48.0f, 36.0f);
}

void Game::Update()
{
  solver.update(0.005f);
	ParticleManager particleManager = solver.getParticleManager();

	Renderer::DrawShapeDuplicate(circle, particleManager.pos); // Draw Border
}

void Game::OnEvent()
{
}