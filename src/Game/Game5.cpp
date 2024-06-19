#include "Game5.h"
#include <algorithm>

Game5::Game5() : LevelLayer("Game5 Layer")
{
}

Game5::~Game5()
{
}

void Game5::OnAttach()
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

	Real resX = 36.0f;
	Real resY = 50.0f;
	solver.initSimulation(resX, resY);

	//Draw level
	solver.drawAngleLineWall(Vec2f(15.0f, 6.0f * resY / 10.0f), 45, 20, 1, true);
	rotatingWallIndex = 0;
	solver.drawAngleLineWall(Vec2f(0, resY - 1), resX * 2, 0, 1);
	solver.drawAngleLineWall(Vec2f(1, 1), resY * 2 - 4, 90, 1);
	solver.drawAngleLineWall(Vec2f(resX, 1), resY * 2 - 4, 90, 1);
	solver.drawRegularGlass(10, 15, Vec2f(2, 10));
	rotatingGlassIndex = 0;
	int width = 20;
	int height = 7;
	solver.drawWinningGlass(width, height, Vec2f(15, 1));

	solver.setSpawnPosition(Vec2f(resX-4, resY - 4));
	solver.spawnLiquidRectangle(Vec2f(resX - 12, resY - 5), 10, 5, 0, ViscosityType::FLUID);

	solver.setGlassSpeed(0.0f, 0.0f);

	solver.init();

	winningGlassParticles = solver.getWinningGlass();
	particleSpawnPosition = solver.getSpawnPosition();

	rectangle = std::make_shared<Rectangle>();
	rectangle->Transform->SetSize(glm::vec2(100.0f));
	Bound bound(glm::vec2(-50.0), glm::vec2(50.0));

	Application::Get()->GetCameraController()->SetCameraMovementBound(bound);

	Bound a(glm::vec2(0.1), glm::vec2(1.0));
	Bound b(glm::vec2(0.1), glm::vec2(2.0));
	Bound c(glm::vec2(0.0), glm::vec2(3.0));

	Bound a_inter_b = Bound::Intersection(a, b);
	Bound a_inter_c = Bound::Intersection(a, c);
	Bound b_inter_c = Bound::Intersection(c, b);
}

void Game5::OnDetach()
{
}

void Game5::Update()
{
	Renderer::DrawShape(rectangle);
	Real _dt = 0.0f;
	if (state != GameState::PAUSED) _dt = solver.update();
	if (state == GameState::RUNNING) maxTime -= _dt;
	if (maxTime < 0.0) {
		maxTime = 0.0;
		state = GameState::LOSE;
	}

	timer += _dt;

	if(isRotatingGlass) timerGlass += _dt * rotationDirection;
	solver.rotateGlass(rotatingGlassIndex, 360.0f * timerGlass / (rotateSpeed * 1000), Vec2f(4.5, 10));

	vector<Particle> particleManager = solver.getParticleManager();

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

	int particlesInGlass = solver.getParticlesInGlass();
	if (particlesInGlass >= winningGlassParticles)
	{
		circleGlass->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		state = GameState::WIN;
	}
}

bool Game5::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius), ViscosityType::FLUID);
		if (keypressed.GetKey() == CORE_KEY_LEFT) {
			isRotatingGlass = true;
			rotationDirection = 1;
		}
		if (keypressed.GetKey() == CORE_KEY_RIGHT) {
			isRotatingGlass = true;
			rotationDirection = -1;
		}


		return true;
	}
	else if (e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) isRotatingGlass = false;
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) isRotatingGlass = false;

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