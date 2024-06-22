#pragma once

#include "../Core/Engine.h"
#include "../Simulator/Solver.h"

class LevelLayer : public Layer
{
	struct LevelInfo
	{
		glm::ivec2 DensityTextureSize;
		uint32_t ParticleCount;
		uint32_t DensityZoomFactor;
		uint32_t BackgroundZoomFactor;
	};
public:
	LevelLayer(const std::string& name, const Bound& levelBound);
	LevelLayer(const std::string& name) : Layer(name), m_DensityCompute(ComputeShader("")), m_ResetCompute(ComputeShader("")), m_SpatialHashCompute(ComputeShader("")), m_FluidRenderingCompute(ComputeShader("")) {}
	~LevelLayer() {}

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	virtual void Update() override;
	virtual void UpdateGame() = 0;
	virtual bool OnEvent(Event& e) = 0;

	virtual float getTime() const = 0;

	GameState getState() const { return m_State; }

	void setPaused() { m_State = GameState::PAUSED; }
	void setRunning() { m_State = GameState::RUNNING; }

protected:
	GameState m_State = GameState::RUNNING;

	Bound m_Bound;
	glm::ivec2 m_Size;
	Solver m_Solver;
	std::vector<Particle> m_Particles;

	std::shared_ptr<Circle> m_WallParticle;
	uint32_t m_MaxParticle = 1000;
private:
	std::shared_ptr<Rectangle> m_Background;
	ComputeShader m_ResetCompute;
	ComputeShader m_DensityCompute;
	ComputeShader m_SpatialHashCompute;
	ComputeShader m_FluidRenderingCompute;
	std::vector<glm::uvec4> m_HashTable;

	std::shared_ptr<RenderTexture2D> m_DensityTexture;
	std::shared_ptr<RenderTexture2D> m_BackgroundTexture;

	DataBufferObject m_BackgroundData;
	DataBufferObject m_BackgroundHashTable;

	LevelInfo m_LevelInfo;
	DataBufferObject m_LevelInfoBuffer;

};