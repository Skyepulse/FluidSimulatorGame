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

    void onEvent(Event &e);

private:
    void buildMenu();
    void reloadFonts();
    void buildInGame();
    void buildPause();
    void buildPopUp(const char* message, const char* subMessage, ImVec4 color, ImVec4 hoverColor, void (*onClick)());

    // 0: Menu, 1: Game, 2: Pause
    int state=0;
    int windowWidth, windowHeight;
};