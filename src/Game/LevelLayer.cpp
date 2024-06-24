#include "LevelLayer.h"

LevelLayer::LevelLayer(const std::string& name, const Bound& levelBound, bool drawWalls)
	: Layer(name), m_Bound(levelBound), m_Solver(Solver()), m_DensityCompute("src/Game/Compute/density.comp"), m_ResetCompute("src/Game/Compute/resetTex.comp"), m_SpatialHashCompute("src/Game/Compute/SpatialGrid.comp"), m_PartCountCompute("src/Game/Compute/partCount.comp")
{
	m_Size = glm::ivec2(levelBound.MaxCorner - levelBound.MinCorner);
	
	InitComputeRect(m_StaticWallParticleRect, m_StaticWallParticleTexture, levelBound);
	InitComputeRect(m_MovingWallParticleRect, m_MovingWallParticleTexture, levelBound);
	InitComputeRect(m_GlassParticleRect, m_GlassParticleTexture, levelBound);
	InitComputeRect(m_RigidbodyParticleRect, m_RigidbodyParticleTexture, levelBound);
	InitComputeRect(m_FluidParticleRect, m_FluidParticleTexture, levelBound);

	m_ParticlePositionBuffer = DataBufferObject(m_Solver.getParticleManager().data(), m_MaxParticle * sizeof(Vec2f));
	m_ParticleViscosityBuffer = DataBufferObject(m_Solver.getParticleManager().data(), m_MaxParticle * sizeof(int));
	m_ColorPerViscosity = {
		glm::vec4(0.0, 0.902, 1.0, 0.8),
		glm::vec4(0.0, 0.447, 0.839, 0.8),
		glm::vec4(1.0, 0.0, 1.0, 0.8),
		glm::vec4(1.0, 0.0, 0.0, 0.8),
	};
	m_ParticleViscosityColorBuffer = DataBufferObject(m_ColorPerViscosity.data(), m_ColorPerViscosity.size() * sizeof(glm::vec4));

	m_HashTable = std::vector<glm::uvec4>(m_MaxParticle);
	m_BackgroundHashBuffer = DataBufferObject(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfo = LevelInfo();
	m_LevelInfo.LowDensityColor = glm::vec4(0.0, 0.902, 1.0, 1.0);
	m_LevelInfo.HighDensityColor = glm::vec4(0, 0.447, 0.839, 1.0);
	m_LevelInfo.LowDensityThreshold = 0.5;
	m_LevelInfo.DensityTextureSize = m_MovingWallParticleTexture->GetSize(); // StaticParticleTexture has same size
	m_LevelInfo.ParticleCount = m_MaxParticle;
	m_LevelInfo.DensityZoomFactor = m_MovingWallParticleTexture->GetWidth() / m_Size.x; // StaticParticleTexture has same size
	m_LevelInfoBuffer = DataBufferObject(&m_LevelInfo, sizeof(LevelInfo));

	glm::vec2 BoundSize = m_Bound.GetSize();
	m_Solver.initSimulation(BoundSize.x, BoundSize.y);
	if (drawWalls) m_Solver.drawWalls(BoundSize.x, BoundSize.y);

	resX = BoundSize.x;
	resY = BoundSize.y;

	Application::Get()->GetCameraController()->SetCameraMovementBound(m_Bound);
	Application::Get()->GetCameraController()->SetCameraPosition(levelBound.MaxCorner / 2.0f + levelBound.MinCorner);
	Application::Get()->GetCameraController()->AdaptZoomLevel(m_Bound);

	ComputeStaticWallParticleTexture();

	previousTime = Time::GetSeconds();
	accumulator = 0.0;

	pipeTex = std::make_shared<Texture2D>("src/data/pipe.png");
	pipe = std::make_shared<Rectangle>();
	pipe->SetTexture(pipeTex.get());
	pipe->Transform->Scale2D(glm::vec2(5.0, 5.0 * 50.0 / 15.0));
	pipe->Transform->Translate2D(glm::vec2(0, 9));
}

void LevelLayer::Update()
{
	HandleFramerate();
	CheckPlayerWin();

	m_Particles = m_Solver.getParticleManager();

	ComputeMovingWallParticleTexture();
	ComputeGlassParticleTexture();
	ComputeRigidBodyParticleTexture();
	ComputeFluidParticleTexture();

	Renderer::DrawShape(m_FluidParticleRect);

	Renderer::DrawShape(m_RigidbodyParticleRect);
	Renderer::DrawShape(m_GlassParticleRect);

	Renderer::DrawShape(m_MovingWallParticleRect);
	Renderer::DrawShape(m_StaticWallParticleRect);

	UpdateGame();
}

void LevelLayer::HandleFramerate()
{
	_dt = 0.0f;
	double currentTime = Time::GetSeconds();
	double frameTime = currentTime - previousTime;
	previousTime = currentTime;
	accumulator += frameTime;

	if (!SHOULD_FPSCAP) accumulator = 0.0;

	while (accumulator >= MIN_FRAME_TIME && Time::GetSeconds() - currentTime < 0.5f * MIN_FRAME_TIME) {
		if (m_State != GameState::PAUSED) {
			double step = m_Solver.update();
			accumulator -= step;
			_dt += step;
		}
		else {
			accumulator = 0.0;
			break;
		}
	}
	if (m_State == GameState::RUNNING) maxTime -= _dt;

	if (accumulator > MIN_FRAME_TIME) return;
}

void LevelLayer::CheckPlayerWin()
{
	if (m_State != GameState::RUNNING && m_State != GameState::PAUSED) return;
	if(m_Solver.isInLoseZone())
	{
		m_State = GameState::LOSE;
		return;
	}

	if (maxTime < 0.0) {
		maxTime = 0.0;
		m_State = GameState::LOSE;
		return;
	}

	int particlesInGlass = m_Solver.getParticlesInGlass();
	if (particlesInGlass >= winningGlassParticles)
	{
		m_GlassColor = m_WinningGlassColor;
		m_State = GameState::WIN;
		return;
	}
}

void LevelLayer::InitComputeRect(std::shared_ptr<Rectangle>& rect, std::shared_ptr<RenderTexture2D>& renderTexture, const Bound& bound)
{
	renderTexture = std::make_shared<RenderTexture2D>(2 * m_Size.x, 2 * m_Size.y);

	rect = std::make_shared<Rectangle>();
	rect->Transform->SetSize(m_Size);
	rect->Transform->SetPosition2D(bound.MaxCorner / 2.0f + bound.MinCorner);
	rect->SetTexture(renderTexture.get());
}

/// <summary>
/// To be called in init
/// </summary>
void LevelLayer::ComputeStaticWallParticleTexture()
{
	m_Particles = m_Solver.getParticleManager();
	std::vector<Vec2f> pos;

	for (const Particle& p : m_Particles)
		pos.push_back(p.pos);

	// Update color and threshold for walls
	m_LevelInfo.LowDensityColor = glm::vec4(1.0);

	DispatchComputes(m_StaticWallParticleTexture, pos);
}

void LevelLayer::ComputeMovingWallParticleTexture()
{
	std::vector<Vec2f> pos;

	for (ParticleGroup& pGroup : m_Solver._wallGroups)
	{
		for (int i = pGroup.startIdx; i < pGroup.endIdx; i++)
			pos.push_back(m_Particles[i].pos);
	}

	// Update color and threshold for dynamic walls
	m_LevelInfo.LowDensityColor = glm::vec4(1.0);

	DispatchComputes(m_MovingWallParticleTexture, pos);
}

void LevelLayer::ComputeGlassParticleTexture()
{
	std::vector<Vec2f> pos;

	for (ParticleGroup& pGroup : m_Solver._glassGroups)
	{
		for (int i = pGroup.startIdx; i < pGroup.endIdx; i++)
			pos.push_back(m_Particles[i].pos);
	}

	// Update color and threshold for glass particle
	m_LevelInfo.LowDensityColor = m_GlassColor;

	DispatchComputes(m_GlassParticleTexture, pos);
}

void LevelLayer::ComputeRigidBodyParticleTexture()
{
	std::vector<Vec2f> pos;

	for (ParticleGroup& pGroup : m_Solver._rigidBodies)
	{
		for (int i = pGroup.startIdx; i < pGroup.endIdx; i++)
			pos.push_back(m_Particles[i].pos);
	}

	// Update color and threshold for rigidbody particle
	m_LevelInfo.LowDensityColor = glm::vec4(0.0, 1.0, 0.0, 1.0);

	DispatchComputes(m_RigidbodyParticleTexture, pos);
}

void LevelLayer::ComputeFluidParticleTexture()
{
	std::vector<Vec2f> pos;
	std::vector<int> viscosityType;

	for (const Particle& p : m_Particles)
		if (p.type == 0)
		{
			pos.push_back(p.pos);
			viscosityType.push_back(p.viscosityType);
		}

	// Update color and threshold for fluid particle
	m_LevelInfo.LowDensityColor = glm::vec4(0.0, 0.902, 1.0, 1.0);
	m_LevelInfo.HighDensityColor = glm::vec4(0, 0.447, 0.839, 1.0);
	m_LevelInfo.LowDensityThreshold = 0.05;

	DispatchComputes(m_FluidParticleTexture, pos, viscosityType);
}

void LevelLayer::DispatchComputes(std::shared_ptr<RenderTexture2D> renderTexture, const std::vector<Vec2f>& particlePositions, const std::vector<int>& particleViscType)
{
	renderTexture->Bind(0);

	m_ParticlePositionBuffer.Bind(1);
	m_ParticlePositionBuffer.UpdateData(particlePositions.data(), particlePositions.size() * sizeof(Vec2f));

	m_BackgroundHashBuffer.Bind(2);
	m_ParticlePositionBuffer.UpdateData(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfoBuffer.Bind(3);
	m_LevelInfo.ParticleCount = particlePositions.size();
	m_LevelInfoBuffer.UpdateData(&m_LevelInfo);

	// Add particle viscosityy
	m_ParticleViscosityBuffer.Bind(4);
	m_ParticleViscosityBuffer.UpdateData(particleViscType.data(), particleViscType.size() * sizeof(int));

	m_ParticleViscosityColorBuffer.Bind(5);

	m_ResetCompute.Dispatch((renderTexture->GetWidth() + 31) / 32, (renderTexture->GetHeight() + 31) / 32);
	m_SpatialHashCompute.Dispatch((particlePositions.size() + 31) / 32, 1);
	m_DensityCompute.Dispatch((renderTexture->GetWidth() + 31) / 32, (renderTexture->GetHeight() + 31) / 32);

	renderTexture->Unbind();
	m_ParticlePositionBuffer.Unbind();
	m_BackgroundHashBuffer.Unbind();
	m_LevelInfoBuffer.Unbind();
}


void LevelLayer::DispatchComputes(std::shared_ptr<RenderTexture2D> renderTexture, const std::vector<Vec2f>& particlePositions)
{
	renderTexture->Bind(0);

	m_ParticlePositionBuffer.Bind(1);
	m_ParticlePositionBuffer.UpdateData(particlePositions.data(), particlePositions.size() * sizeof(Vec2f));

	m_BackgroundHashBuffer.Bind(2);
	m_ParticlePositionBuffer.UpdateData(m_HashTable.data(), m_MaxParticle * sizeof(glm::uvec4));

	m_LevelInfoBuffer.Bind(3);
	m_LevelInfo.ParticleCount = particlePositions.size();
	m_LevelInfoBuffer.UpdateData(&m_LevelInfo);

	m_ResetCompute.Dispatch((renderTexture->GetWidth() + 31) / 32, (renderTexture->GetHeight() + 31) / 32);
	m_PartCountCompute.Dispatch((particlePositions.size() + 31) / 32, 1);

	renderTexture->Unbind();
	m_ParticlePositionBuffer.Unbind();
	m_BackgroundHashBuffer.Unbind();
	m_LevelInfoBuffer.Unbind();
}
