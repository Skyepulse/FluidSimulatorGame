#include "Game9.h"
#include <algorithm>

Game9::Game9() : LevelLayer("Game Layer", Bound(glm::vec2(36.0f, 50.0f)))
{
}

Game9::~Game9()
{
}

void Game9::OnAttach()
{
	Real resX = 36.0f;
	Real resY = 50.0f;
	_resX = resX;
	_resY = resY;

	//Draw level
	m_Solver.drawAngleLineWall(Vec2f(26, 0), 60, 90);
	m_Solver.drawAngleLineWall(Vec2f(0, 20), 50, 0);

	int width = 10;
	int height = 10;
	m_Solver.drawWinningGlass(width, height, Vec2f(1, 1));

	m_Solver.addRigidBody(Vec2f(28, 1), width, height, 10);
	m_Solver.setDefaultDt(0.01f);

	m_Solver.setSpawnPosition(Vec2f(resX-4, resY - 4));

	pipe->Transform->Translate2D(glm::vec2(resX-4, resY - 4));

	m_Solver.init();

	winningGlassParticles = m_Solver.getWinningGlass();
	particleSpawnPosition = m_Solver.getSpawnPosition();

	_glassRange = resY / 8 * 0.8;

	startTime = maxTime;

	objectiveTex = std::make_shared<Texture2D>("src/data/rigidHint.png");
	objective = std::make_shared<Rectangle>();
	objective->SetTexture(objectiveTex.get());
	objective->Transform->Scale2D(glm::vec2(width-1, 7));
	objective->Transform->SetPosition2D(glm::vec2(30.5, 20));

	Application::Get()->GetUI()->setHintMessage("Press <- or -> to move the pipe !");
}

void Game9::OnDetach()
{
}

void Game9::UpdateGame()
{
	Vec2f wallPos = m_Solver.getWallPos(1);

	if (m_Solver.getRigidPosition(0).y > 18.5 && m_Solver.getRigidPosition(0).y < 21.5){
		m_Solver.moveWall(1, Vec2f(-2, 0));
	} else {
		m_Solver.moveWall(1, Vec2f(4, 0));
	}

	Vec2f wallVel = m_Solver.getWallVel(1);

	if (wallPos.x < -25 && wallVel.x < 0) m_Solver.moveWall(1, Vec2f(0, 0));
	if (wallPos.x > 0.1 && wallVel.x > 0) m_Solver.moveWall(1, Vec2f(0, 0));

	Renderer::DrawShape(objective);
	Renderer::DrawShape(pipe);
}

bool Game9::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::KeyPressed) {
		KeyPressedEvent& keypressed = dynamic_cast<KeyPressedEvent&>(e);

		glm::vec2 pos = pipe->Transform->GetPosition();

		if (keypressed.GetKey() == CORE_KEY_LEFT && pos.x > 4) {
			pipe->Transform->Translate2D(glm::vec2(-1, 0));
			glm::vec2 newPos = pipe->Transform->GetPosition();
			particleSpawnPosition = Vec2f(newPos.x, newPos.y - 9);		
		}
		if (keypressed.GetKey() == CORE_KEY_RIGHT && pos.x < resX - 4){ 
			pipe->Transform->Translate2D(glm::vec2(1, 0));
			glm::vec2 newPos = pipe->Transform->GetPosition();
			particleSpawnPosition = Vec2f(newPos.x, newPos.y - 9);
		}

		if (keypressed.GetKey() == CORE_KEY_P) {
			m_Solver.spawnParticle(particleSpawnPosition, particleSpawnRadius, ViscosityType::FLUID, Vec2f(0, -10));
		}

		return true;
	}
	return false;
}

Vec2f Game9::getRandomPointInCircle(const Vec2f& center, const Real radius)
{
	Real angle = (Real)rand() / RAND_MAX * 2 * M_PI;
	Real r = radius * sqrt((Real)rand() / RAND_MAX);
	Real x = r * cos(angle);
	Real y = r * sin(angle);

	return Vec2f(center.x + x, center.y + y);
}