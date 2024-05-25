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
  Solver solver;

  std::shared_ptr<Circle> circleWalls;
  std::shared_ptr<Circle> circleLiquid;
};