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

private:
  Vec2f getRandomPointInCircle(const Vec2f& center, const Real radius);

  //std::shared_ptr<Circle> circleWalls;
  std::shared_ptr<Circle> circleLiquid;
  std::shared_ptr<Circle> circleGlass;

  Vec2f particleSpawnPosition = Vec2f(0.0f, 0.0f);
  Real particleSpawnRadius = 4.0f;

  bool moveGlassUp = false;	
  bool moveGlassDown = false;
  bool moveGlassLeft = false;
  bool moveGlassRight = false;
  Real _moveGlassSpeedX = 8.1f; // per second so dt 1000
  Real _moveGlassSpeedY = 8.1f; // per second so dt 1000
  int winningGlassIndex = 0;
};