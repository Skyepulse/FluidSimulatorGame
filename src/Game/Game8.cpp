#include "Game8.h"
#include <algorithm>

Game8::Game8() : LevelLayer("Game Layer", Bound(glm::vec2(36.0, 50.0)))
{
}

Game8::~Game8()
{
}

void Game8::OnAttach()
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

	Real resX = 50.0f;
	Real resY = 50.0f;
	_resX = resX;
	_resY = resY;

	//Draw level

	m_Solver.drawAngleLineWall(Vec2f(0, resY - 17), 40, 0);
	m_Solver.drawAngleLineWall(Vec2f(20, resY - 17), 20, 90);

	m_Solver.drawAngleLineWall(Vec2f(20, resY - 28), 30, 0);
	m_Solver.drawAngleLineWall(Vec2f(20, 0), 30, 90);

	int width = 18;
	int height = 18;
	glassWidth = width;
	glassHeight = height;
	m_Solver.setSpawnPosition(Vec2f(resX - 4, resY - 4));

	m_Solver.drawWinningGlass(width, height, Vec2f(20, 1));
	m_Solver.activateInfiniteWalls();
	m_Solver.spawnLiquidRectangle(Vec2f(2, resY - 12), 10, 10, 0, ViscosityType::FLUID);

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();


	arrowTex = std::make_shared<Texture2D>("src/data/arrow.png");
	arrow = std::make_shared<Rectangle>();
	arrow->SetTexture(arrowTex.get());
	arrow->Transform->Scale2D(glm::vec2(8.0, 8.0 * 9.0 / 14.0));
	arrow->Transform->Rotate2D(-M_PI_2);

	arrowPos.emplace_back(-10, resY/2);
	arrowPos.emplace_back(resX - 4, resY/2);


	Application::Get()->GetUI()->setHintMessage("Press G to change gravity !");
}

void Game8::OnDetach()
{
}

void Game8::UpdateGame()
{
	Vec2f velVec = Vec2f(0, 0);
	if (_moveGlassLeft) velVec.x -= _glassSpeedX;
	if (_moveGlassRight) velVec.x += _glassSpeedX;
	m_Solver.moveGlass(winningGlassIndex, velVec, true);

	Renderer::DrawShapeDuplicate(arrow, arrowPos);
}

bool Game8::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		Vec2f g = m_Solver.getGravity();

		if (keypressed.GetKey() == CORE_KEY_G){ 
			g.rotate90();
			arrow->Transform->Rotate2D(M_PI_2);
		}

		m_Solver.setGravity(g);

		return true;
	}

	return false;
}

Vec2f Game8::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}