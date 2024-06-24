#pragma once

#include "LevelLayer.h"

class Game12 : public LevelLayer
{
public:
	Game12();
	~Game12();

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

	bool moveGlassUp = false;
	bool moveGlassDown = false;
	bool moveGlassLeft = false;
	bool moveGlassRight = false;
	Real _moveGlassSpeedX = 8.1f; // per second so dt 1000
	Real _moveGlassSpeedY = 8.1f; // per second so dt 1000
	int winningGlassIndex = 0;

	int _maxParticles;
};