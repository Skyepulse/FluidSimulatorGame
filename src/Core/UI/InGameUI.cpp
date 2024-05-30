#include "InGameUI.h"

InGameUI::~InGameUI(){
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void InGameUI::init(GLFWwindow *window){
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	// Initialisation backend (GLFW et OpenGL3)
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void InGameUI::newFrame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void InGameUI::draw(){

    ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Text("Hello, world!");
    if (ImGui::Button("Button"))
    {
        // Action du bouton
    }
    ImGui::End();

    // Rendre ImGui par-dessus la scène OpenGL
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}