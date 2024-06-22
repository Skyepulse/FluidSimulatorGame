#include "LevelLayer.h"

LevelLayer::LevelLayer(const std::string& name, const Bound& levelBound)
	: Layer(name), m_Bound(levelBound), m_Solver(Solver()), m_DensityCompute("src/Game/Compute/density.comp"), m_ResetCompute("src/Game/Compute/resetTex.comp"), m_SpatialHashCompute("src/Game/Compute/SpatialGrid.comp"), m_FluidRenderingCompute("src/Game/Compute/fluidRendering.comp")
{
	m_Size = glm::ivec2(levelBound.MaxCorner - levelBound.MinCorner);
	m_DensityTexture = std::make_shared<RenderTexture2D>(2 * m_Size.x, 2 * m_Size.y);
	//m_BackgroundTexture = std::make_shared<RenderTexture2D>(10 * m_Size.x, 10 * m_Size.y);

	m_Background = std::make_shared<Rectangle>();
	m_Background->Transform->SetSize(m_Size);
	m_Background->Transform->Translate2D(levelBound.MaxCorner / 2.0f + levelBound.MinCorner);
	m_Background->SetTexture(m_DensityTexture.get());
	m_BackgroundData = DataBufferObject(m_Solver.getParticleManager().data(), m_MaxParticle * sizeof(Particle));

	m_HashTable = std::vector<glm::uvec4>(m_MaxParticle);
	m_BackgroundHashTable = DataBufferObject(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfo = LevelInfo();
	m_LevelInfo.DensityTextureSize = m_DensityTexture->GetSize();
	m_LevelInfo.ParticleCount = m_MaxParticle;
	m_LevelInfo.DensityZoomFactor = m_DensityTexture->GetWidth() / m_Size.x;
	m_LevelInfoBuffer = DataBufferObject(&m_LevelInfo, sizeof(LevelInfo));

	m_WallParticle = std::make_shared<Circle>();
	m_WallParticle->Transform->Scale2D(0.5f);
	m_WallParticle->SetColor(glm::vec3(0.6f));

	glm::vec2 BoundSize = m_Bound.GetSize();
	m_Solver.initSimulation(BoundSize.x, BoundSize.y);
	m_Solver.drawWalls(BoundSize.x, BoundSize.y);

	Application::Get()->GetCameraController()->SetCameraMovementBound(m_Bound);
	Application::Get()->GetCameraController()->SetCameraPosition(levelBound.MaxCorner / 2.0f + levelBound.MinCorner);
}

void LevelLayer::Update()
{
	static int p = 0;
	m_Particles = m_Solver.getParticleManager();
	std::vector<Vec2f> pos;

	for (const Particle& p : m_Particles)
	{
		if (p.type == 0)
		{
			pos.push_back(p.pos);
		}
	}

	m_DensityTexture->BindImage(0);

	m_BackgroundData.Bind(1);
	m_BackgroundData.UpdateData(pos.data(), pos.size() * sizeof(Vec2f));

	m_BackgroundHashTable.Bind(2);
	m_BackgroundData.UpdateData(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfoBuffer.Bind(3);
	m_LevelInfo.ParticleCount = pos.size();
	m_LevelInfoBuffer.UpdateData(&m_LevelInfo);

	//m_BackgroundTexture->BindImage(4);

	m_ResetCompute.Dispatch((m_DensityTexture->GetWidth() + 31) / 32, (m_DensityTexture->GetHeight() + 31) / 32);
	m_SpatialHashCompute.Dispatch((pos.size() + 31) / 32, 1);
	m_DensityCompute.Dispatch((m_DensityTexture->GetWidth() + 31) / 32, (m_DensityTexture->GetHeight() + 31) / 32);

	//m_FluidRenderingCompute.Dispatch((m_BackgroundTexture->GetWidth() + 31) / 32, (m_BackgroundTexture->GetHeight() + 31) / 32);

	//m_DensityTexture->UnbindImage();
	m_BackgroundData.Unbind();
	m_BackgroundHashTable.Unbind();
	m_LevelInfoBuffer.Unbind();

	//m_BackgroundTexture->BindImage(0);
	Renderer::DrawShape(m_Background);

	UpdateGame();
}