#pragma once

#include "../Simulator/Solver.h"
#include "../Core/Scene/Circle.h"
#include "../Core/Event/Event.h"

// TEMP : TODO : REPLACE BY LAYERS
class Game
{
public:
  Game();
  ~Game();

  void Init();
  void Update();

  bool OnEvent(Event& e);
private:
	Vec2f getRandomPointInCircle(const Vec2f& center, const Real radius);
  Solver solver;
  int winningGlassParticles;

  std::shared_ptr<Circle> circleWalls;
  std::shared_ptr<Circle> circleLiquid;
  std::shared_ptr<Circle> circleGlass;

  Vec2f particleSpawnPosition = Vec2f(0.0f, 0.0f);
  Real particleSpawnRadius = 4.0f;
};