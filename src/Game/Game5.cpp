#include "Game5.h"
#include <algorithm>

Game5::Game5() : LevelLayer("Game5 Layer", Bound(glm::vec2(36.0, 50.0)), false)
{
}

Game5::~Game5()
{
}

void Game5::OnAttach()
{
	circleDebug = std::make_shared<Circle>();
	float circleRadius = 0.5f;
	circleDebug->Transform->Scale2D(circleRadius);
	circleDebug->SetColor(glm::vec3(0.7f, 0.7f, 0.0f));

	Real resX = 36.0f;
	Real resY = 50.0f;

	//Draw level
	m_Solver.drawAngleLineWall(Vec2f(15.0f, 6.0f * resY / 10.0f), 45, 20, 1, true);
	rotatingWallIndex = 0;
	m_Solver.drawAngleLineWall(Vec2f(0, resY - 1), resX * 2, 0, 1);
	m_Solver.drawAngleLineWall(Vec2f(1, 1), resY * 2 - 4, 90, 1);
	m_Solver.drawAngleLineWall(Vec2f(resX, 1), resY * 2 - 4, 90, 1);
	m_Solver.drawRegularGlass(10, 15, Vec2f(2, 10));
	rotatecenter = Vec2f(4.5, 10);
	rotatingGlassIndex = 0;
	int width = 20;
	int height = 7;
	m_Solver.drawWinningGlass(width, height, Vec2f(15, 1));

	m_Solver.setSpawnPosition(Vec2f(resX-4, resY - 4));
	m_Solver.spawnLiquidRectangle(Vec2f(resX - 12, resY - 5), 10, 5, 0, ViscosityType::FLUID);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();

	Application::Get()->GetUI()->setHintMessage("Press C or B to rotate the glass !");
}

void Game5::OnDetach()
{
}

void Game5::UpdateGame()
{	
	rotatecenter += _dt * velVec;

	circleDebug->Transform->SetPosition2D(glm::vec2(rotatecenter.x, rotatecenter.y));
	Renderer::DrawShape(circleDebug);

	if (isRotatingGlass) {
		timerGlass += _dt * rotationDirection;
		m_Solver.rotateGlass(rotatingGlassIndex, 360.0f * timerGlass / (rotateSpeed * 1000), rotatecenter);
	}

	velVec = Vec2f(0.0f, 0.0f);
	if (moveIntermGlassLeft) velVec.x -= moveSpeedGlass;
	if (moveIntermGlassRight) velVec.x += moveSpeedGlass;
	if (moveIntermGlassUp) velVec.y += moveSpeedGlass;
	if (moveIntermGlassDown) velVec.y -= moveSpeedGlass;
	m_Solver.moveGlass(rotatingGlassIndex, velVec);
}

bool Game5::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT 
			&& keypressed.GetKey() != CORE_KEY_UP && keypressed.GetKey() != CORE_KEY_DOWN
			&& keypressed.GetKey() != CORE_KEY_C && keypressed.GetKey() != CORE_KEY_B)
			return false;

		if (keypressed.GetKey() == CORE_KEY_LEFT) moveIntermGlassLeft = true;
		if (keypressed.GetKey() == CORE_KEY_RIGHT) moveIntermGlassRight = true;
		if (keypressed.GetKey() == CORE_KEY_UP) moveIntermGlassUp = true;
		if (keypressed.GetKey() == CORE_KEY_DOWN) moveIntermGlassDown = true;
		if (keypressed.GetKey() == CORE_KEY_C) {
			isRotatingGlass = true;
			rotationDirection = 1;
		}
		if (keypressed.GetKey() == CORE_KEY_B) {
			isRotatingGlass = true;
			rotationDirection = -1;
		}


		return true;
	}
	else if (e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT 
			&& keyreleased.GetKey() != CORE_KEY_UP && keyreleased.GetKey() != CORE_KEY_DOWN
			&& keyreleased.GetKey() != CORE_KEY_C && keyreleased.GetKey() != CORE_KEY_B)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) moveIntermGlassLeft = false;
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) moveIntermGlassRight = false;
		if (keyreleased.GetKey() == CORE_KEY_UP) moveIntermGlassUp = false;
		if (keyreleased.GetKey() == CORE_KEY_DOWN) moveIntermGlassDown = false;
		if (keyreleased.GetKey() == CORE_KEY_C || keyreleased.GetKey() == CORE_KEY_B) {
			isRotatingGlass = false;
		}

		return true;
	}

	return false;
}

Vec2f Game5::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}