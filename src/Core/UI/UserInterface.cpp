#include "UserInterface.h"
#include <cstdio>
#include "../Application.h"

UserInterface::~UserInterface(){
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UserInterface::init(Window *window){
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	// Initialisation backend (GLFW et OpenGL3)
	ImGui_ImplGlfw_InitForOpenGL(window->Get(), true);
	ImGui_ImplOpenGL3_Init("#version 130");

    windowWidth = window->GetWidth();
    windowHeight = window->GetHeight();

    io.Fonts->AddFontFromFileTTF("src/font.ttf", 30);
}

void UserInterface::newFrame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UserInterface::show(){
    ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    
    switch (state)
    {
    case 0:
        buildMenu();
        break;
    case 1:
        buildInGame();
        break;
    case 2:
        buildPause();
        break;
    
    default:
        break;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::buildMenu(){

    // Dimensions de la grille
    int rows = 4;
    int cols = 3;
    int buttonIndex = 1;

    // Dimensions des boutons
    ImVec2 buttonSize(100, 100);

    // Espacement entre les boutons
    ImVec2 buttonSpacing(20, 20);

    // Calculer la taille totale de la grille
    float gridWidth = cols * buttonSize.x + (cols - 1) * buttonSpacing.x;
    float gridHeight = rows * buttonSize.y + (rows - 1) * buttonSpacing.y;

    // Calculer la position de départ pour centrer la grille
    ImVec2 startPos((windowWidth - gridWidth) / 2, (windowHeight - gridHeight) / 2);

    // Configurer le style pour les boutons ronds
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 50.0f; // Arrondir les coins des boutons

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            // Calculer la position du bouton
            ImVec2 buttonPos = ImVec2(
                startPos.x + col * (buttonSize.x + buttonSpacing.x),
                startPos.y + row * (buttonSize.y + buttonSpacing.y)
            );

            // Définir la position du curseur pour placer le bouton
            ImGui::SetCursorPos(buttonPos);

            // Créer le bouton avec le numéro
            char buttonLabel[32];
            snprintf(buttonLabel, sizeof(buttonLabel), "%d", buttonIndex);
            if (ImGui::Button(buttonLabel, buttonSize))
            {
                state = 1;
            }

            ++buttonIndex;
        }
    }
}


void UserInterface::buildInGame(){
    ImGui::SetCursorPos(ImVec2(windowWidth - 67, 0));
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 30.0f;
    if (ImGui::Button("| |", ImVec2(60, 60))) {
        state = 2;
    }
}

void UserInterface::buildPause(){
    // Dimensions des boutons
    ImVec2 buttonSize(200, 50);

    // Espacement entre les boutons
    float buttonSpacing = 20.0f;

    // Calculer la position de départ pour centrer les boutons
    float totalHeight = 3 * buttonSize.y + 2 * buttonSpacing;
    ImVec2 startPos((windowWidth - buttonSize.x) / 2, (windowHeight - totalHeight) / 2);

    // Configurer le style pour les boutons avec des bords arrondis en demi-cercle
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = buttonSize.y / 2.0f; // Arrondir les coins des boutons pour faire des demi-cercles

    // Placer et dessiner chaque bouton
    ImGui::SetCursorPos(startPos);
    if (ImGui::Button("Resume", buttonSize))
    {
        state = 1;
    }

    ImGui::SetCursorPos(ImVec2(startPos.x, startPos.y + buttonSize.y + buttonSpacing));
    if (ImGui::Button("Restart", buttonSize))
    {
        state = 1;
    }

    ImGui::SetCursorPos(ImVec2(startPos.x, startPos.y + 2 * (buttonSize.y + buttonSpacing)));
    if (ImGui::Button("Menu", buttonSize))
    {
        state = 0;
    }
}