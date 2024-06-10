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
        Application::PushLayer(new Game());
    }    
};

Application* Application::Create() { return new GameApplication(); } 
