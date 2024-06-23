#pragma once

#include "GameLayer.h"

class Game7 : public LevelLayer
{
public:
	Game7();
	~Game7();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void Update() override;
	virtual bool OnEvent(Event& e) override;

	float getTime() const override { return maxTime; }

private:
	Vec2f getRandomPointInCircle(const Vec2f& center, const Real radius);
	Solver solver;
	int winningGlassParticles;

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

	float maxTime = 100.0f;
};