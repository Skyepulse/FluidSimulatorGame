#pragma once

#include "../Core/Engine.h"
#include "../Simulator/Solver.h"

class LevelLayer : public Layer
{
public:
	//LevelLayer(const Bound& levelBound, const Solver* solver) : m_Bound(levelBound), m_Solver(solver) {}
	LevelLayer(const std::string& name) : Layer(name) {}
	~LevelLayer() {}

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	virtual void Update() = 0;
	virtual bool OnEvent(Event& e) = 0;

	virtual float getTime() const = 0;

	GameState getState() const { return state; }

	void setPaused() { state = GameState::PAUSED; }
	void setRunning() { state = GameState::RUNNING; }
protected:
	GameState state = GameState::RUNNING;
private:
	Bound m_Bound;
	Solver* m_Solver;
};