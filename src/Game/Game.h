#pragma once

#include "LevelLayer.h"

class Game : public LevelLayer
{
public:
  Game();
  ~Game();

  virtual void OnAttach() override;
  virtual void OnDetach() override;

  virtual void UpdateGame() override;
  virtual bool OnEvent(Event& e) override;

  float getTime() const override { return maxTime; }

private:
  Vec2f getRandomPointInCircle(const Vec2f& center, const Real radius);
  int winningGlassParticles;

  //std::shared_ptr<Circle> circleWalls;
  std::shared_ptr<Circle> circleLiquid;
  std::shared_ptr<Circle> circleGlass;

  Vec2f particleSpawnPosition = Vec2f(0.0f, 0.0f);
  Real particleSpawnRadius = 4.0f;

  float maxTime = 100.0f;
};