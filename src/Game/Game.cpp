#include "Game.h"

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
  circle  = std::make_shared<Circle>();
	float circleRadius = 0.2f;
	circle->Transform->Scale2D(circleRadius);
	circle->SetColor(glm::vec3(1.0f));

	solver.initSimulation(48.0f, 100.0f);
}

void Game::Update()
{
  solver.update(0.005f);
	ParticleManager particleManager = solver.getParticleManager();

	Renderer::DrawShapeDuplicate(circle, particleManager.pos);
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