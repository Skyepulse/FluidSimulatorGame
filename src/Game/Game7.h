#pragma once

#include "LevelLayer.h"

class Game7 : public LevelLayer
{
public:
	Game7();
	~Game7();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void UpdateGame() override;
	virtual bool OnEvent(Event& e) override;

private:
	Vec2f getRandomPointInCircle(const Vec2f& center, const Real radius);

	std::shared_ptr<Circle> circleWalls;
	std::shared_ptr<Circle> circleLiquid;
	std::shared_ptr<Circle> circleGlass;
	std::shared_ptr<Circle> circleViscousLiquid;

	std::shared_ptr<Rectangle> rectangle;

	Vec2f particleSpawnPosition = Vec2f(0.0f, 0.0f);
	Real particleSpawnRadius = 4.0f;

	bool moveGlassUp = false;
	bool moveGlassDown = false;
	bool moveGlassLeft = false;
	bool moveGlassRight = false;
	Real _moveGlassSpeedX = 4.0f; // per second so dt 1000
	Real _moveGlassSpeedY = 4.0f; // per second so dt 1000
	int winningGlassIndex = 0;

	int barrier1Index = 0;
	int barrier2Index = 1;
};