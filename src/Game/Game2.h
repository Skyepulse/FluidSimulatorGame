#pragma once

#include "../Simulator/Solver.h"
#include "../Core/Engine.h"

// TEMP : TODO : REPLACE BY LAYERS
class Game2 : public Layer
{
public:
	Game2();
	~Game2();

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

	Vec2f particleSpawnPosition = Vec2f(0.0f, 0.0f);
	Real particleSpawnRadius = 4.0f;
	Real _resX;
	Real _resY;
	Real glassHeight = 0.0f;
	Real glassWidth = 0.0f;

	int _maxParticles;

	float maxTime = 100.0f;
};