#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <string>
#include <iostream>

#include "Core/OpenGL/Context.h"
#include "Core/OpenGL/Shader.h"
#include "Core/OpenGL/Buffer.h"
#include "Core/OpenGL/VertexArray.h"
#include "Core/OpenGL/Texture.h"


#include "Core/Rendering/Camera.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/RendererCommand.h"
#include "Core/Rendering/Transform.h"

#include "Core/Scene/Circle.h"
#include "Core/Scene/Line.h"

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Time.h"
#include "Core/Window.h"

#include "Simulator/Solver.h"

int main() {
	// Init of logger
	Logger::Init();
	
	// ---------------------------------------------------------------------------
	//															GLFW / GLAD INIT
	// ---------------------------------------------------------------------------

	std::shared_ptr<WindowProperties> windowProperties = std::make_shared<WindowProperties>();
	windowProperties->Width = 800;
	windowProperties->Height = 600;
	std::shared_ptr<Window> window = std::make_shared<Window>(windowProperties);
	
	// TEMP A CHANGER
	// Enable Blending
	RendererCommand::EnableBlending(true);

	// ---------------------------------------------------------------------------
	//																	DATA INIT
	// ---------------------------------------------------------------------------

	// Solver
	Solver solver;
	solver.initSimulation(48.0f, 36.0f);
	ParticleManager particleManager;
	
	Camera camera(0.0f, 48.0f * solver.getH() *2, 0.0f, 36.0f * solver.getH()*2); // MUltiply by h

	// Particle representation
	std::shared_ptr<Circle> circle  = std::make_shared<Circle>();
	float circleRadius = 0.2f;
	circle->Transform->Scale2D(circleRadius);
	circle->Transform->GetTranslated(glm::vec3(10.0));
	circle->SetColor(glm::vec3(1.0f));

	// END CIRCLE

	solver.init();
	
	
	int numUpdates = 3;
	while (!window->ShouldClose()) {
		//CORE_INFO(Time::GetDeltaTime());


		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		Renderer::BeginScene(camera);

		particleManager = solver.getParticleManager();

		if(numUpdates > 0) solver.update();
		
		Renderer::DrawShapeDuplicate(circle, particleManager.pos); // Draw Border

		Renderer::EndScene();

		window->OnUpdate();
		numUpdates--;
	}

	return 0;
}