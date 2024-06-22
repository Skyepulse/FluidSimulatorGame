#include "LevelLayer.h"

LevelLayer::LevelLayer(const std::string& name, const Bound& levelBound)
	: Layer(name), m_Bound(levelBound), m_Solver(Solver()), m_DensityCompute("src/Game/Compute/density.comp"), m_ResetCompute("src/Game/Compute/resetTex.comp"), m_SpatialHashCompute("src/Game/Compute/SpatialGrid.comp")
{
	m_Size = glm::ivec2(levelBound.MaxCorner - levelBound.MinCorner);
	m_MovingParticleTexture = std::make_shared<RenderTexture2D>(2 * m_Size.x, 2 * m_Size.y);
	m_StaticParticleTexture = std::make_shared<RenderTexture2D>(2 * m_Size.x, 2 * m_Size.y);

	m_MovingParticleRect = std::make_shared<Rectangle>();
	m_MovingParticleRect->Transform->SetSize(m_Size);
	m_MovingParticleRect->Transform->Translate2D(levelBound.MaxCorner / 2.0f + levelBound.MinCorner);
	m_MovingParticleRect->SetTexture(m_MovingParticleTexture.get());

	m_StaticParticleRect = std::make_shared<Rectangle>();
	m_StaticParticleRect->Transform->SetSize(m_Size);
	m_StaticParticleRect->Transform->Translate2D(levelBound.MaxCorner / 2.0f + levelBound.MinCorner);
	m_StaticParticleRect->SetTexture(m_StaticParticleTexture.get());

	m_BackgroundData = DataBufferObject(m_Solver.getParticleManager().data(), m_MaxParticle * sizeof(Particle));

	m_HashTable = std::vector<glm::uvec4>(m_MaxParticle);
	m_BackgroundHashTable = DataBufferObject(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfo = LevelInfo();
	m_LevelInfo.LowDensityColor = glm::vec4(0.0, 0.902, 1.0, 1.0);
	m_LevelInfo.HighDensityColor = glm::vec4(0, 0.447, 0.839, 1.0);
	m_LevelInfo.LowDensityThreshold = 0.5;
	m_LevelInfo.DensityTextureSize = m_MovingParticleTexture->GetSize(); // StaticParticleTexture has same size
	m_LevelInfo.ParticleCount = m_MaxParticle;
	m_LevelInfo.DensityZoomFactor = m_MovingParticleTexture->GetWidth() / m_Size.x; // StaticParticleTexture has same size
	m_LevelInfoBuffer = DataBufferObject(&m_LevelInfo, sizeof(LevelInfo));

	m_WallParticle = std::make_shared<Circle>();
	m_WallParticle->Transform->Scale2D(0.5f);
	m_WallParticle->SetColor(glm::vec3(0.6f));

	glm::vec2 BoundSize = m_Bound.GetSize();
	m_Solver.initSimulation(BoundSize.x, BoundSize.y);
	m_Solver.drawWalls(BoundSize.x, BoundSize.y);

	Application::Get()->GetCameraController()->SetCameraMovementBound(m_Bound);
	Application::Get()->GetCameraController()->SetCameraPosition(levelBound.MaxCorner / 2.0f + levelBound.MinCorner);

	m_Particles = m_Solver.getParticleManager();
	std::vector<Vec2f> pos;

	for (const Particle& p : m_Particles)
	{
		if (p.type == 1)
			pos.push_back(p.pos);
	}

	m_StaticParticleTexture->Bind(0);

	m_BackgroundData.Bind(1);
	m_BackgroundData.UpdateData(pos.data(), pos.size() * sizeof(Vec2f));

	m_BackgroundHashTable.Bind(2);
	m_BackgroundData.UpdateData(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfoBuffer.Bind(3);
	// Color borders in white
	m_LevelInfo.LowDensityColor = glm::vec4(1.0);
	m_LevelInfo.HighDensityColor = glm::vec4(1.0);
	m_LevelInfo.ParticleCount = pos.size();
	m_LevelInfoBuffer.UpdateData(&m_LevelInfo);

	m_ResetCompute.Dispatch((m_StaticParticleTexture->GetWidth() + 31) / 32, (m_StaticParticleTexture->GetHeight() + 31) / 32);
	m_SpatialHashCompute.Dispatch((pos.size() + 31) / 32, 1);
	m_DensityCompute.Dispatch((m_StaticParticleTexture->GetWidth() + 31) / 32, (m_StaticParticleTexture->GetHeight() + 31) / 32);

	m_StaticParticleTexture->Unbind();
	m_BackgroundData.Unbind();
	m_BackgroundHashTable.Unbind();
	m_LevelInfoBuffer.Unbind();

	// Reset level infos
	m_LevelInfo.LowDensityColor = glm::vec4(0.0, 0.902, 1.0, 1.0);
	m_LevelInfo.HighDensityColor = glm::vec4(0, 0.447, 0.839, 1.0);
	m_LevelInfo.LowDensityThreshold = 0.05;
}

void LevelLayer::Update()
{
	m_Particles = m_Solver.getParticleManager();
	std::vector<Vec2f> pos;

	for (const Particle& p : m_Particles)
	{
		if (p.type == 0)
		{
			pos.push_back(p.pos);
		}
	}

	m_MovingParticleTexture->Bind(0);

	m_BackgroundData.Bind(1);
	m_BackgroundData.UpdateData(pos.data(), pos.size() * sizeof(Vec2f));

	m_BackgroundHashTable.Bind(2);
	m_BackgroundData.UpdateData(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfoBuffer.Bind(3);
	m_LevelInfo.ParticleCount = pos.size();
	m_LevelInfoBuffer.UpdateData(&m_LevelInfo);

	m_ResetCompute.Dispatch((m_MovingParticleTexture->GetWidth() + 31) / 32, (m_MovingParticleTexture->GetHeight() + 31) / 32);
	m_SpatialHashCompute.Dispatch((pos.size() + 31) / 32, 1);
	m_DensityCompute.Dispatch((m_MovingParticleTexture->GetWidth() + 31) / 32, (m_MovingParticleTexture->GetHeight() + 31) / 32);

	m_MovingParticleTexture->Unbind();
	m_BackgroundData.Unbind();
	m_BackgroundHashTable.Unbind();
	m_LevelInfoBuffer.Unbind();

	Renderer::DrawShape(m_MovingParticleRect);
	Renderer::DrawShape(m_StaticParticleRect);

	UpdateGame();
}