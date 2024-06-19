#pragma once

#include "GameLayer.h"

class Game4 : public LevelLayer
{
public:
	Game4();
	~Game4();

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

	float maxTime = 100.0f;
};