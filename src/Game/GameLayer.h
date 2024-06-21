#pragma once

#include "../Core/Engine.h"
#include "../Simulator/Solver.h"
#include "../Core/Time.h"

class LevelLayer : public Layer
{
public:
	//LevelLayer(const Bound& levelBound, const Solver* solver) : m_Bound(levelBound), m_Solver(solver) {}
	LevelLayer(const std::string& name) : Layer(name), previousTime(Time::GetSeconds()), accumulator(0) {}
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
	double previousTime;

	const double MIN_FRAME_TIME = 1.0 / 30.0;
	double accumulator;

private:
	Bound m_Bound;
	Solver* m_Solver;
};