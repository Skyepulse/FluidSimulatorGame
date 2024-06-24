#include "UserInterface.h"
#include <cstdio>
#include "../Application.h"
#include <cmath>
#include <algorithm>
#include "../../Simulator/kernel.h"
#include "../Time.h"


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

    reloadFonts();
}

void UserInterface::reset(){
    shouldRestart = true;
    setHintMessage("");
}

void UserInterface::reloadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    float fontSize = std::min(windowHeight, windowWidth) / 6.0f;

    io.Fonts->AddFontFromFileTTF("src/data/font.ttf", fontSize*0.3f);
    io.Fonts->AddFontFromFileTTF("src/data/font.ttf", fontSize*0.5f);
    io.Fonts->AddFontFromFileTTF("src/data/font.ttf", fontSize);
    io.Fonts->AddFontFromFileTTF("src/data/font.ttf", fontSize*0.4f);

    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

void UserInterface::newFrame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UserInterface::show(){
    ImGui::SetNextWindowPos(ImVec2(3, 3), ImGuiCond_Always);
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

void UserInterface::setHintMessage(const char *hint){
    strcpy(hintString, hint);
}

void UserInterface::buildMenu(){

    // Dimensions de la grille
    int rows = 3;
    int cols = 4;
    int buttonIndex = 1;

    // Dimensions des boutons
    ImVec2 buttonSize(std::min(windowWidth, windowHeight)/5.0f, std::min(windowWidth, windowHeight)/5.0f);

    // Espacement entre les boutons
    ImVec2 buttonSpacing(buttonSize.x * 0.25, buttonSize.y * 0.2);

    // Title
    ImGuiIO& io = ImGui::GetIO();
    ImFont* titleFont = io.Fonts->Fonts[2];
    ImGui::PushFont(titleFont);

    const char* title = "GlassOverflow";
    ImVec2 titleSize = ImGui::CalcTextSize(title);
    ImVec2 titlePos((windowWidth - titleSize.x) / 2, 50);
    ImGui::SetCursorPos(titlePos);

    ImVec4 defaultColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    ImVec4 highlightColor = ImVec4(80.0f/255.0f, 188.0f / 255.0f, 216.0f / 255.0f, 255.0f / 255.0f); //Bleu

    ImGui::BeginGroup(); 
    for (int i = 0; title[i] != '\0'; ++i) {
        if (title[i] == 'O' || title[i] == 'G') {
            ImGui::PushStyleColor(ImGuiCol_Text, highlightColor);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, defaultColor);
        }

        ImGui::TextUnformatted(&title[i], &title[i] + 1);
        ImGui::SameLine(0.0f, 0.0f);

        ImGui::PopStyleColor();
    }
    ImGui::EndGroup();

    ImGui::PopFont();

    // Calculer la taille totale de la grille
    float gridWidth = cols * buttonSize.x + (cols - 1) * buttonSpacing.x;
    float gridHeight = rows * buttonSize.y + (rows - 1) * buttonSpacing.y;
    float gridOffset = -150.0f;

    // Calculer la position de départ pour centrer la grille
    ImVec2 startPos((windowWidth - gridWidth) / 2.0f, (windowHeight - gridHeight - gridOffset) / 2.0f);

    // Configurer le style pour les boutons ronds
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 100.0f; // Arrondir les coins des boutons

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
                Application* app = Application::Get();
                if(app != nullptr) app->loadGame(buttonIndex);
            }

            ++buttonIndex;
        }
    }
}

void UserInterface::buildHint(const char* message){

    float time = Time::GetSeconds();

    if (shouldRestart) {
        hintStartTime = time;
        shouldRestart = false;
    }
    CORE_DEBUG(time - hintStartTime)
    if (time - hintStartTime > HINT_DURATION) return;

    ImVec2 textSize = ImGui::CalcTextSize(message);
    ImVec2 textPos((windowWidth - textSize.x) / 2, (windowHeight - textSize.y) / 2);
    ImGui::SetCursorPos(textPos);

    float alpha = (std::sin(time * 2.0f * M_PI * 0.5f) * 0.5f) + 0.5f;

    ImVec4 textColor(1.0f, 1.0f, 1.0f, alpha);
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);

    ImGui::Text("%s", message);

    ImGui::PopStyleColor();
}

void UserInterface::buildPopUp(const char* message, const char* subMessage, ImVec4 color, ImVec4 hoverColor, void (*onClick)()){
    ImVec2 rectSize(std::min(windowHeight, windowWidth)/2.5f, std::min(windowHeight, windowWidth) / 5.0f);

    ImVec2 rectPos((windowWidth - rectSize.x) / 2, (windowHeight - rectSize.y) / 2);

    ImGuiIO& io = ImGui::GetIO();
    ImFont* smallFont = io.Fonts->Fonts[0];
    ImFont* bigFont = io.Fonts->Fonts[1];

    ImVec2 textSize = ImGui::CalcTextSize(message);
    ImVec2 nextLevelTextSize;

    ImGui::PushFont(bigFont);
    textSize = ImGui::CalcTextSize(message);
    ImGui::PopFont();

    ImGui::PushFont(smallFont);
    nextLevelTextSize = ImGui::CalcTextSize(subMessage);
    ImGui::PopFont();

    ImVec2 textPos(rectPos.x + (rectSize.x - textSize.x) / 2, rectPos.y + (rectSize.y - textSize.y) / 2);
    ImVec2 nextLevelTextPos(rectPos.x + (rectSize.x - nextLevelTextSize.x) / 2, rectPos.y + (rectSize.y + textSize.y) / 2);

    ImGui::SetCursorPos(rectPos);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);

    if (ImGui::Button("##", rectSize)){
        onClick();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    ImGui::SetCursorPos(textPos);
    ImGui::PushFont(bigFont);
    ImGui::Text("%s", message);
    ImGui::PopFont();

    ImGui::SetCursorPos(nextLevelTextPos);
    ImGui::PushFont(smallFont);
    ImGui::Text("%s", subMessage);
    ImGui::PopFont();
}


void UserInterface::buildInGame(){
    ImGui::SetCursorPos(ImVec2(windowWidth - 67, 0));
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 30.0f;
    if (ImGui::Button("| |", ImVec2(60, 60))) {
        Application::Get()->pauseGame();
        state = 2;
    }

    ImGui::SetCursorPos(ImVec2(windowWidth - 130, 0));
    if (ImGui::Button("?", ImVec2(60, 60))) {
        hintStartTime = -1;
    }

    Application* app = Application::Get();

    int timer = ceil(app->getGameTime());

    char numberText[8];
    snprintf(numberText, sizeof(numberText), "%d", timer);
    ImVec2 textSize = ImGui::CalcTextSize(numberText);
    ImGui::SetCursorPos(ImVec2((windowWidth - textSize.x) / 2, 10));
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->Fonts[3];
    ImGui::PushFont(font);
    ImGui::Text("%s", numberText);
    ImGui::PopFont();

    if (app->getGameState() == GameState::WIN){
        buildPopUp("YOU WON!", "Next level", ImVec4(0.2f, 0.6f, 0.2f, 1.0f), ImVec4(0.3f, 0.7f, 0.3f, 1.0f), []() {Application::Get()->loadNextGame();});
    } else if (app->getGameState() == GameState::LOSE){
        buildPopUp("YOU LOSE!", "Restart level", ImVec4(0.6f, 0.2f, 0.2f, 1.0f), ImVec4(0.7f, 0.3f, 0.3f, 1.0f), []() {Application::Get()->restartGame();});
    }

    buildHint(hintString);
}

void UserInterface::buildPause(){
    // Dimensions des boutons
    ImVec2 buttonSize(std::min(windowHeight, windowWidth) / 3.0f, std::min(windowHeight, windowWidth) / 12.0f);

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
        Application::Get()->resumeGame();
        state = 1;
    }

    ImGui::SetCursorPos(ImVec2(startPos.x, startPos.y + buttonSize.y + buttonSpacing));
    if (ImGui::Button("Restart", buttonSize))
    {
        Application::Get()->restartGame();
        state = 1;
    }

    ImGui::SetCursorPos(ImVec2(startPos.x, startPos.y + 2 * (buttonSize.y + buttonSpacing)));
    if (ImGui::Button("Menu", buttonSize))
    {
        Application::Get()->unloadGame();
        state = 0;
    }
}

void UserInterface::onEvent(Event &e){
    if (e.GetEventType() == EventType::WindowResized) {
		WindowResizedEvent& e_win = dynamic_cast<WindowResizedEvent&>(e);
        glm::vec2 size = e_win.GetSize();

        windowWidth = size.x;
        windowHeight = size.y;

        reloadFonts();
    }
}