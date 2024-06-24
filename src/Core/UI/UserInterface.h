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

    void setHintMessage(const char *hint);

    void reset();

private:
    void buildMenu();
    void reloadFonts();
    void buildInGame();
    void buildPause();
    void buildPopUp(const char* message, const char* subMessage, ImVec4 color, ImVec4 hoverColor, void (*onClick)());
    void buildHint(const char* message);

    // 0: Menu, 1: Game, 2: Pause
    int state=0;
    int windowWidth, windowHeight;
    char hintString[100];
    float hintStartTime=0;
    bool shouldRestart = false;

    const float HINT_DURATION = 5.0;
};