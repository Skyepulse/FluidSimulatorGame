#pragma once

#include "LevelLayer.h"

class Game8 : public LevelLayer
{
public:
	Game8();
	~Game8();

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

	int winningGlassIndex = 0;
	Real _glassSpeedY = 2.1f;
	Real _glassSpeedX = 6.1f;
	bool _moveGlassLeft = false;
	bool _moveGlassRight = false;

	int _maxParticles;

	std::shared_ptr<Texture2D> arrowTex;
	std::shared_ptr<Shape> arrow;
	std::vector<glm::vec2> arrowPos;

};