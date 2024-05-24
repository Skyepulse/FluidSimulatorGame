#pragma once

#include "../Simulator/Solver.h"
#include "../Core/Scene/Circle.h"

// TEMP : TODO : REPLACE BY LAYERS
class Game
{
public:
  Game();
  ~Game();

  void Init();
  void Update();

  void OnEvent();
private:
  Solver solver;

  std::shared_ptr<Circle> circle;
};