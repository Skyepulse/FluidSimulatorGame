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
	solver.drawAngleLineWall(Vec2f(20, 3 * resY / 10), 45, 30, 1);
	int width = 10;
	int height = 10;
	solver.drawWinningGlass(width, height, Vec2f(1, 1));
	solver.setSpawnPosition(Vec2f(4, resY - 4));

	solver.spawnLiquidRectangle(Vec2f(2, resY - 15), 10, 10);

	solver.setGlassSpeed(4.0f, 0.0f);

	solver.init();

	winningGlassParticles = solver.getWinningGlass();
	particleSpawnPosition = solver.getSpawnPosition();

	rectangle = std::make_shared<Rectangle>();
	rectangle->Transform->Scale2D(50.0f);
	Bound bound(glm::vec2(-50.0), glm::vec2(50.0));
	Application::Get()->GetCameraController()->LimitCameraMovementInBound(true);
	Application::Get()->GetCameraController()->SetCameraMovementBound(bound);

/*	Bound a(glm::vec2(0.1), glm::vec2(1.0));
	Bound b(glm::vec2(0.1), glm::vec2(2.0));
	Bound c(glm::vec2(0.0), glm::vec2(3.0));

	DEBUG("0,0 is in a : {}", a.InBound(glm::vec2(0)));
	DEBUG("1,1 is in a : {}", a.InBound(glm::vec2(1)));
	DEBUG("1,1 is in b : {}", b.InBound(glm::vec2(1)));

	DEBUG("a is included in b : {}", b.Includes(a)); // false
	DEBUG("a is included in c : {}", c.Includes(a)); // true
	DEBUG("b is included in c : {}", c.Includes(b)); // true*/
}

void Game::OnDetach()
{
}

void Game::Update()
{
	Renderer::DrawShape(rectangle);
	Real _dt = 0.0f;
	if(state != GameState::PAUSED) _dt = solver.update();
	if(state == GameState::RUNNING) maxTime -= _dt;
	if (maxTime < 0.0) {
		maxTime = 0.0;
		state = GameState::LOSE;
	}

	vector<Particle> particleManager = solver.getParticleManager();

	vector<Vec2f> wallsPositions;
	vector<Vec2f> liquidPositions;
	vector<Vec2f> glassPositions;

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

		if (keypressed.GetKey() != CORE_KEY_P && keypressed.GetKey() != CORE_KEY_LEFT && keypressed.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keypressed.GetKey() == CORE_KEY_P) solver.spawnParticle(getRandomPointInCircle(particleSpawnPosition, particleSpawnRadius));
		if (keypressed.GetKey() == CORE_KEY_LEFT) solver.moveGlassLeft(true);
		if (keypressed.GetKey() == CORE_KEY_RIGHT) solver.moveGlassRight(true);


		return true;
	} else if(e.GetEventType() == EventType::KeyReleased) {
		KeyReleasedEvent& keyreleased = dynamic_cast<KeyReleasedEvent&>(e);

		if (keyreleased.GetKey() != CORE_KEY_LEFT && keyreleased.GetKey() != CORE_KEY_RIGHT)
			return false;

		if (keyreleased.GetKey() == CORE_KEY_LEFT) solver.moveGlassLeft(false);
		if (keyreleased.GetKey() == CORE_KEY_RIGHT) solver.moveGlassRight(false);

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