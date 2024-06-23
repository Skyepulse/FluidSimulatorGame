#pragma once

#include "../Core/Engine.h"
#include "../Simulator/Solver.h"

class LevelLayer : public Layer
{
	struct LevelInfo
	{
		glm::vec4 LowDensityColor;
		glm::vec4 HighDensityColor;
		glm::ivec2 DensityTextureSize;
		uint32_t ParticleCount;
		uint32_t DensityZoomFactor;
		uint32_t BackgroundZoomFactor;
		float LowDensityThreshold;
	};
public:
	LevelLayer(const std::string& name, const Bound& levelBound, bool drawWalls=true);
	~LevelLayer() {}

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	virtual void Update() override;
	virtual void UpdateGame() = 0;
	virtual bool OnEvent(Event& e) = 0;

	float getTime() const { return maxTime; };

	GameState getState() const { return m_State; }

	void setPaused() { m_State = GameState::PAUSED; }
	void setRunning() { m_State = GameState::RUNNING; }
private:
	void HandleFramerate();
	void CheckPlayerWin();

	void InitComputeRect(std::shared_ptr<Rectangle>& rect, std::shared_ptr<RenderTexture2D>& renderTexture, const Bound& bound);
	void ComputeStaticWallParticleTexture();
	void ComputeMovingWallParticleTexture();
	void ComputeGlassParticleTexture();
	void ComputeRigidBodyParticleTexture();
	void ComputeFluidParticleTexture();
	void DispatchComputes(std::shared_ptr<RenderTexture2D> renderTexture, const std::vector<Vec2f>& particlePositions, bool useDensity = true);
protected:
	GameState m_State = GameState::RUNNING;

	Bound m_Bound;
	glm::ivec2 m_Size;
	Solver m_Solver;
	std::vector<Particle> m_Particles;

	uint32_t m_MaxParticle = 1000;

	int winningGlassParticles;
private:
	glm::vec4 m_GlassColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
	glm::vec4 m_WinningGlassColor = glm::vec4(0.0, 1.0, 0.0, 1.0);

	double previousTime;
	float maxTime = 100.0f;

	const double MIN_FRAME_TIME = 1.0f / 30.0f;
	double accumulator;
	const bool SHOULD_FPSCAP = true;

	std::shared_ptr<Rectangle> m_MovingWallParticleRect;
	std::shared_ptr<Rectangle> m_StaticWallParticleRect;
	std::shared_ptr<Rectangle> m_GlassParticleRect;
	std::shared_ptr<Rectangle> m_RigidbodyParticleRect;
	std::shared_ptr<Rectangle> m_FluidParticleRect;

	ComputeShader m_ResetCompute;
	ComputeShader m_DensityCompute;
	ComputeShader m_PartCountCompute;
	ComputeShader m_SpatialHashCompute;

	std::shared_ptr<RenderTexture2D> m_MovingWallParticleTexture;
	std::shared_ptr<RenderTexture2D> m_StaticWallParticleTexture;
	std::shared_ptr<RenderTexture2D> m_GlassParticleTexture;
	std::shared_ptr<RenderTexture2D> m_RigidbodyParticleTexture;
	std::shared_ptr<RenderTexture2D> m_FluidParticleTexture;

	std::vector<glm::uvec4> m_HashTable;
	LevelInfo m_LevelInfo;

	DataBufferObject m_BackgroundData;
	DataBufferObject m_BackgroundHashTable;
	DataBufferObject m_LevelInfoBuffer;
};