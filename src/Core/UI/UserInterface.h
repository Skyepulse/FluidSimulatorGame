#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class UserInterface {
public:
    UserInterface() {};
    ~UserInterface();
    
    void init(GLFWwindow *window);
    void newFrame();
    void show();
};