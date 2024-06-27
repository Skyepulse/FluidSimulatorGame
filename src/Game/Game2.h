#pragma once

#include "LevelLayer.h"

class Game2 : public LevelLayer
{
public:
	Game2();
	~Game2();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void UpdateGame() override;
	virtual bool OnEvent(Event& e) override;
private:
	Vec2f getRandomPointInCircle(const Vec2f& center, const Real radius);

	Vec2f particleSpawnPosition = Vec2f(0.0f, 0.0f);
	Real particleSpawnRadius = 4.0f;
	Real _resX;
	Real _resY;
	Real glassHeight = 0.0f;
	Real glassWidth = 0.0f;

	int glassIndex = 0;
	float _glassPeriod = 5.0;
	float _glassRange;
	float startTime;

	int _maxParticles;
};