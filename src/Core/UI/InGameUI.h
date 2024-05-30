#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class InGameUI {
    InGameUI() {};
    ~InGameUI();
    
    void init(GLFWwindow *window);
    void newFrame();
    void draw();
};