#pragma once

#include "../Core/Engine.h"

#include "Game.h"
#include "Game2.h"
#include "Game3.h"

class GameApplication : public Application
{
public: 
    GameApplication() 
    {
    }   

    void loadGame(int index) override {
		unloadGame();
		switch (index)
		{
		case 1:
			m_game = new Game();
			break;
		case 2:
			m_game = new Game2();
			break;
		case 3:
			m_game = new Game3();
			break;
		default:
			m_game = new Game();
			break;
		}
		game_index = index;

		PushLayer(m_game);
	}

	void unloadGame() override {
		if (m_game != nullptr) {
			PopLayer(m_game);
		}	
	}

	void restartGame() override {
		loadGame(game_index);
	}

	float getGameTime() override {
		return m_game->getTime();
	}

	void pauseGame() override {
		m_game->setPaused();
	}

	void resumeGame() override {
		m_game->setRunning();
	}

private:
	Layer* m_game = nullptr;
	int game_index = 0;
};

Application* Application::Create() { s_Instance = new GameApplication(); return s_Instance; }