#pragma once

#include "LevelLayer.h"

class Game5 : public LevelLayer
{
public:
	Game5();
	~Game5();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void UpdateGame() override;
	virtual bool OnEvent(Event& e) override;

private:
	Vec2f getRandomPointInCircle(const Vec2f& center, const Real radius);

	int rotatingWallIndex = 0;
	float rotateSpeed = 1.0f; // time to rotate 360 degrees in seconds
	float timer = 0.0f;

	int rotatingGlassIndex = 0;
	float rotateSpeedGlass = 8.0f; // time to rotate 360 degrees in seconds
	float moveSpeedGlass = 8.0f; // time to move 1 unit in seconds
	float timerGlass = 0.0f;
	Vec2f velVec = Vec2f(0.0f, 0.0f);
	bool isRotatingGlass = false;
	int rotationDirection = 1;
	bool moveIntermGlassUp = false;
	bool moveIntermGlassDown = false;
	bool moveIntermGlassLeft = false;
	bool moveIntermGlassRight = false;
	Vec2f rotatecenter = Vec2f(0.0f, 0.0f);

	std::shared_ptr<Circle> circleDebug;

	Vec2f particleSpawnPosition = Vec2f(0.0f, 0.0f);
	Real particleSpawnRadius = 4.0f;
};