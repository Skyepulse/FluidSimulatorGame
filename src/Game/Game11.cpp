#include "Game11.h"
#include <algorithm>

Game11::Game11() : LevelLayer("Game11 Layer", Bound(glm::vec2(36.0, 50.0)))
{
}

Game11::~Game11()
{
}

void Game11::OnAttach()
{
	Real resX = 36.0f;
	Real resY = 50.0f;
	//Draw level
	m_Solver.drawAngleLineWall(Vec2f(0, 8.5 * resY / 10), 50, -30, 1);
	m_Solver.drawAngleLineWall(Vec2f(6, 2.5 * resY / 10), 52, 30, 1);
	m_Solver.drawAngleLineWall(Vec2f(resX - 8, resY - 1), 46, -90, 1);
	int width = 14;
	int height = 10;
	regularGlassIndex = 0;
	m_Solver.drawWinningGlass(width, height, Vec2f(1, 1));

	m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 5), 4, 3, 0, ViscosityType::FLUID);
	
	m_Solver.setLoseZone(Vec2f(0.0f, 2.0f), Vec2f(resX, 0.0f));

	m_Solver.setSpawnPosition(Vec2f(resX - 4, resY - 4));

	pipe->Transform->Translate2D(glm::vec2(resX - 4, resY - 4));

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();

	GlassMinX = 1;
	GlassMaxX = -width / 2.0f - 1;

	Application::Get()->GetUI()->setHintMessage("Don't spill the water! Change glass velocity with up and down arrows!");

}

void Game11::OnDetach()
{
}

void Game11::UpdateGame()
{
	Vec2f velVec = Vec2f(0, 0);
	Vec2f pos = m_Solver.getGlassPosition();
	if (moveGlassLeft && pos.x > GlassMinX) velVec.x -= _moveGlassSpeedX;
	if (moveGlassRight && pos.x < resX + GlassMaxX) velVec.x += _moveGlassSpeedX;
	if (moveGlassUp) velVec.y += _moveGlassSpeedY;
	if (moveGlassDown) velVec.y -= _moveGlassSpeedY;
	m_Solver.moveGlass(regularGlassIndex, velVec, true);

	Renderer::DrawShape(pipe);
}

bool Game11::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT
			&& keypressed.GetKey() != CORE_KEY_UP && keypressed.GetKey() != CORE_KEY_DOWN && keypressed.GetKey() != CORE_KEY_P)
			return false;

		if (keypressed.GetKey() == CORE_KEY_LEFT) moveGlassLeft = true;
		if (keypressed.GetKey() == CORE_KEY_RIGHT) moveGlassRight = true;
		if (keypressed.GetKey() == CORE_KEY_UP) SpeedUpGlass();
		if (keypressed.GetKey() == CORE_KEY_DOWN) SpeedDownGlass();

		if (keypressed.GetKey() == CORE_KEY_P) {
			m_Solver.spawnParticle(particleSpawnPosition, particleSpawnRadius, ViscosityType::FLUID, Vec2f(0, -10));
		}

		return true;
	}
	else if (e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT
			&& keyreleased.GetKey() != CORE_KEY_UP && keyreleased.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) moveGlassLeft = false;
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) moveGlassRight = false;

		return true;
	}

	return false;
}

Vec2f Game11::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}