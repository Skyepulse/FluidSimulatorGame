#pragma once

#include "../Simulator/Solver.h"
#include "../Core/Engine.h"

// TEMP : TODO : REPLACE BY LAYERS
class Game : public Layer
{
public:
  Game();
  ~Game();

  virtual void Init() override;
  virtual void Update() override;

  virtual bool OnEvent(Event& e) override;

  virtual void OnAttach() override;
  virtual void OnDetach() override;
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