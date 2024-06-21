#include "Game.h"
#include <algorithm>

Game::Game() : LevelLayer("Game Layer")
{
}

Game::~Game()
{
}

void Game::OnAttach()
{
	circleWalls = std::make_shared<Circle>();
	circleLiquid = std::make_shared<Circle>();
	circleGlass = std::make_shared<Circle>();


	float circleRadius = 0.5f;
	circleWalls->Transform->Scale2D(circleRadius);
	circleLiquid->Transform->Scale2D(circleRadius);
	circleGlass->Transform->Scale2D(circleRadius);

	circleWalls->SetColor(glm::vec3(0.6f));
	circleLiquid->SetColor(glm::vec3(0.2f, 0.3f, 1.0f));
	circleGlass->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));

	Real resX = 36.0f;
	Real resY = 50.0f;
	solver.initSimulation(resX, resY);

	//Draw level
	solver.drawWalls(resX, resY);
	solver.drawAngleLineWall(Vec2f(0, 7 * resY/10), 45, -30, 1);
	solver.drawAngleLineWall(Vec2f(20, 3 * resY / 10), 36, 30, 1);
	int width = 10;
	int height = 10;
	solver.drawWinningGlass(width, height, Vec2f(1, 1));

	// solver.addRigidBody(Vec2f(2*width, 5), width, height, 100);

	solver.setSpawnPosition(Vec2f(4, resY - 4));

	solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10, 0, ViscosityType::FLUID);

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

	DEBUG("a inter b : {}, {}; {}, {}", a_inter_b.MinCorner.x, a_inter_b.MinCorner.y, a_inter_b.MaxCorner.x, a_inter_b.MaxCorner.y);
	DEBUG("a inter c : {}, {}; {}, {}", a_inter_c.MinCorner.x, a_inter_c.MinCorner.y, a_inter_c.MaxCorner.x, a_inter_c.MaxCorner.y);
	DEBUG("b inter c : {}, {}; {}, {}", b_inter_c.MinCorner.x, b_inter_c.MinCorner.y, b_inter_c.MaxCorner.x, b_inter_c.MaxCorner.y);
}

void Game::OnDetach()
{
}

void Game::Update()
{
	Renderer::DrawShape(rectangle);

	// ----------- HANDLE FRAMERATE ----------
	Real _dt = 0.0f;
	double currentTime = Time::GetSeconds();
	double frameTime = currentTime - previousTime;
	previousTime = currentTime;
	accumulator += frameTime;

	while (accumulator >= MIN_FRAME_TIME) {
		if(state != GameState::PAUSED){
            double step = solver.update();
            accumulator -= step;
			_dt += step;
        } else {
			accumulator = 0.0;
		}
	}
	if(state == GameState::RUNNING) maxTime -= _dt;
	if (maxTime < 0.0) {
		maxTime = 0.0;
		state = GameState::LOSE;
	}

	if (accumulator > MIN_FRAME_TIME) return;

	// ----------- END HANDLE FRAMERATE ----------

	vector<Particle> particleManager = solver.getParticleManager();

	vector<Vec2f> wallsPositions;
	vector<Vec2f> liquidPositions;
	vector<Vec2f> glassPositions;

	Vec2f velVec = Vec2f(0, 0);
	if (moveGlassLeft) velVec.x -= _moveGlassSpeedX;
	if (moveGlassRight) velVec.x += _moveGlassSpeedX;
	if (moveGlassUp) velVec.y += _moveGlassSpeedY;
	if (moveGlassDown) velVec.y -= _moveGlassSpeedY;
	solver.moveGlass(winningGlassIndex, velVec, true);

	for (size_t i = 0; i < particleManager.size(); i++)
	{
		if (particleManager[i].type == 1)
			wallsPositions.push_back(particleManager[i].pos);
		else if(particleManager[i].type == 0)
			liquidPositions.push_back(particleManager[i].pos);
		else
			glassPositions.push_back(particleManager[i].pos);

	}

	Renderer::DrawShapeDuplicate(circleWalls, wallsPositions);
	Renderer::DrawShapeDuplicate(circleLiquid, liquidPositions);
	Renderer::DrawShapeDuplicate(circleGlass, glassPositions);

	int particlesInGlass = solver.getParticlesInGlass();
	if (particlesInGlass >= winningGlassParticles)
	{
		circleGlass->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		state = GameState::WIN;
	}
}

bool Game::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT
			&& keypressed.GetKey() != CORE_KEY_UP && keypressed.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius), ViscosityType::FLUID);
		if (keypressed.GetKey() == CORE_KEY_LEFT) moveGlassLeft = true;
		if (keypressed.GetKey() == CORE_KEY_RIGHT) moveGlassRight = true;
		if (keypressed.GetKey() == CORE_KEY_UP) moveGlassUp = true;
		if (keypressed.GetKey() == CORE_KEY_DOWN) moveGlassDown = true;

		return true;
	} else if(e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT
			&& keyreleased.GetKey() != CORE_KEY_UP && keyreleased.GetKey() != CORE_KEY_DOWN)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) moveGlassLeft = false;
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) moveGlassRight = false;
		if (keyreleased.GetKey() == CORE_KEY_UP) moveGlassUp = false;
		if (keyreleased.GetKey() == CORE_KEY_DOWN) moveGlassDown = false;

		return true;
	}
	
	return false;
}

Vec2f Game::getRandomPointInCircle(const Vec2f &center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}