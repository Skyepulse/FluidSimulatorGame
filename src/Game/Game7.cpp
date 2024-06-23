#include "Game7.h"
#include <algorithm>

Game7::Game7() : LevelLayer("Game7 Layer", Bound(glm::vec2(50.0f, 50.0f)))
{
}

Game7::~Game7()
{
}

void Game7::OnAttach()
{
	circleWalls = std::make_shared<Circle>();
	circleLiquid = std::make_shared<Circle>();
	circleGlass = std::make_shared<Circle>();
	circleViscousLiquid = std::make_shared<Circle>();


	float circleRadius = 0.5f;
	circleWalls->Transform->Scale2D(circleRadius);
	circleLiquid->Transform->Scale2D(circleRadius);
	circleGlass->Transform->Scale2D(circleRadius);
	circleViscousLiquid->Transform->Scale2D(circleRadius);

	circleWalls->SetColor(glm::vec3(0.6f));
	circleLiquid->SetColor(glm::vec3(0.2f, 0.3f, 1.0f));
	circleGlass->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));
	circleViscousLiquid->SetColor(glm::vec3(0.2f, 0.7f, 0.3f));

	Real resX = 50.0f;
	Real resY = 50.0f;

	//Draw level
	m_Solver.drawAngleLineWall(Vec2f(13.0f, 5.7f * resY / 10.0f), 35, 135, 1);
	m_Solver.drawAngleLineWall(Vec2f(resX-13.0f, 5.5f * resY / 10.0f), 37, 45, 1);
	m_Solver.drawAngleLineWall(Vec2f(7, 4.2f * resY / 10.0f), resX*2 - 28, 0, 1);
	//m_Solver.drawAngleLineWall(Vec2f(7, 5.0f * resY / 10.0f - 1), resX * 2 - 28, 0, 1);


	//Barriers
	barrier1Index = 3;
	_barrier1Center = Vec2f(13.0f, 8.5f * resY / 10.0f);
	m_Solver.drawAngleLineWall(_barrier1Center, 30, -90, 1);
	
	barrier2Index = 4;
	_barrier2Center = Vec2f(resX - 14.0f, 8.5f * resY / 10.0f);
	m_Solver.drawAngleLineWall(_barrier2Center, 30, -90, 1);

	m_Solver.drawAngleLineWall(Vec2f(7, 4.2f * resY / 10.0f), 40, -90, 1);
	
	//Blocker wall
	m_Solver.drawAngleLineWall(Vec2f(resX - 9.0f, 5.0f * resY / 10.0f), 8, 0, 1);

	int width = 10;
	int height = 10;

	winningGlassIndex = 0;
	m_Solver.drawWinningGlass(width, height, Vec2f(16, 1));

	// m_Solver.addRigidBody(Vec2f(2*width, 5), width, height, 100);
	m_Solver.addRigidBody(Vec2f(24.0f,4.4f*resY / 10.0f), 8, 8, 50);
	m_Solver.setDefaultDt(0.01f);

	m_Solver.setSpawnPosition(Vec2f(4, resY - 4));

	m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 6), 10, 5, 0, ViscosityType::FLUID);
	m_Solver.spawnLiquidRectangle(Vec2f(resX - 12, resY - 6), 10, 5, 0, ViscosityType::VISCOUS);

	m_Solver.setViscosityForWin(ViscosityType::FLUID);

	m_Solver.init();

	winningGlassParticles = 10;
	particleSpawnPosition = m_Solver.getSpawnPosition();

	Application::Get()->GetUI()->setHintMessage("Pour just a little blue fluid in the glass!");
}

void Game7::OnDetach()
{
}

void Game7::UpdateGame()
{
	Vec2f velVec = Vec2f(0, 0);
	if (moveGlassLeft) velVec.x -= _moveGlassSpeedX;
	if (moveGlassRight) velVec.x += _moveGlassSpeedX;
	if (moveGlassUp) velVec.y += _moveGlassSpeedY;
	if (moveGlassDown) velVec.y -= _moveGlassSpeedY;
	m_Solver.moveGlass(winningGlassIndex, velVec, true);

	if (moveBarrier1 && _barrier1CurrentAngle < _barrier1MaxAngle)
	{
		_barrier1Timer += _dt;
		_barrier1CurrentAngle = 360.0f * _barrier1Timer / (_moveBarrierSpeed * 1000);
		CORE_DEBUG("Barrier1 angle: {0}", _barrier1CurrentAngle);
		m_Solver.rotateWall(barrier1Index, _barrier1CurrentAngle, _barrier1Center);
	}
	if (moveBarrier2 && _barrier2CurrentAngle > _barrier2MinAngle)
	{
		_barrier2Timer -= _dt;
		_barrier2CurrentAngle = 360.0f * _barrier2Timer / (_moveBarrierSpeed * 1000);
		m_Solver.rotateWall(barrier2Index, _barrier2CurrentAngle, _barrier2Center);
	}

	vector<Particle> particleManager = m_Solver.getParticleManager();

	vector<Vec2f> wallsPositions;
	vector<Vec2f> liquidPositions;
	vector<Vec2f> glassPositions;
	vector<Vec2f> viscousLiquidPositions;

	for (size_t i = 0; i < particleManager.size(); i++)
	{
		if (particleManager[i].type == 1)
			wallsPositions.push_back(particleManager[i].pos);
		else if (particleManager[i].type == 0)
			if (particleManager[i].viscosityType == ViscosityType::FLUID)liquidPositions.push_back(particleManager[i].pos);
			else viscousLiquidPositions.push_back(particleManager[i].pos);
		else
			glassPositions.push_back(particleManager[i].pos);

	}

	Renderer::DrawShapeDuplicate(circleWalls, wallsPositions);
	Renderer::DrawShapeDuplicate(circleLiquid, liquidPositions);
	Renderer::DrawShapeDuplicate(circleGlass, glassPositions);
	Renderer::DrawShapeDuplicate(circleViscousLiquid, viscousLiquidPositions);
}

bool Game7::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT
			&& keypressed.GetKey() != CORE_KEY_UP && keypressed.GetKey() != CORE_KEY_DOWN
			&& keypressed.GetKey() != CORE_KEY_B && keypressed.GetKey() != CORE_KEY_C)
			return false;

		if (keypressed.GetKey() == CORE_KEY_LEFT) moveGlassLeft = true;
		if (keypressed.GetKey() == CORE_KEY_RIGHT) moveGlassRight = true;
		if (keypressed.GetKey() == CORE_KEY_C) moveBarrier1	= true;
		if (keypressed.GetKey() == CORE_KEY_B) moveBarrier2 = true;
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

Vec2f Game7::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}