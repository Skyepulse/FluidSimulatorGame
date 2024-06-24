#pragma once

#include "LevelLayer.h"

class Game11 : public LevelLayer
{
public:
	Game11();
	~Game11();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void UpdateGame() override;
	virtual bool OnEvent(Event& e) override;

	void SpeedUpGlass() { if (_moveGlassSpeedX + _glassSpeedIncrease >= _maxGlassSpeed) return;  _moveGlassSpeedX += _glassSpeedIncrease; };
	void SpeedDownGlass() { if (_moveGlassSpeedX - _glassSpeedIncrease <= _minGlassSpeed) return;  _moveGlassSpeedX -= _glassSpeedIncrease; };

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
	Real _maxGlassSpeed = 12.1f;
	Real _minGlassSpeed = 2.1f;
	Real _glassSpeedIncrease = 0.1f;
	int regularGlassIndex = 0;

	int _maxParticles;
};