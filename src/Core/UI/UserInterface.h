#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../Window.h"

class UserInterface {
public:
    UserInterface() {};
    ~UserInterface();
    
    void init(Window *window);
    void newFrame();
    void show();

private:
    void buildMenu();
    void buildInGame();
    void buildPause();

    // 0: Menu, 1: Game, 2: Pause
    int state=1;
    int windowWidth, windowHeight;
};