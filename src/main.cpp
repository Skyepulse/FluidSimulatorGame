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
#include "Core/Scene/Rectangle.h"

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
	
	Camera camera(0.0f, 48.0f * solver.getH() *2, 0.0f, 36.0f * solver.getH()*2); // Multiply by h

	// GRID

	std::shared_ptr<Line> gridLine = std::make_shared<Line>();
	gridLine->SetColor(glm::vec3(1.0f)); 

	glm::ivec2 gridSize = camera.GetSize();
	glm::ivec2 gridResolution = glm::ivec2(4, 4);

	std::vector<glm::vec2> gridPosition;
	std::vector<glm::vec2> gridDirection;
	std::vector<glm::vec2> gridScale;

	float cellWidth = (float) gridSize.x / ((float) gridResolution.x);
	float cellHeight = (float) gridSize.y / ((float) gridResolution.y);
	
	for (size_t i = 0; i < gridResolution.x; i++)
	{
		gridPosition.push_back(glm::vec2(cellWidth * (float) i, 0.0f));
		gridDirection.push_back(glm::vec2(0.0f, 1.0f));
		gridScale.push_back(glm::vec2(1.0f, gridSize.y));
	}

	for (size_t i = 0; i < gridResolution.y; i++)
	{
		gridPosition.push_back(glm::vec2(0.0f, cellHeight * (float) i));
		gridDirection.push_back(glm::vec2(1.0f, 0.0f));
		gridScale.push_back(glm::vec2(1.0f, gridSize.x));
	}

	// END GRID

	// CELLS

	std::shared_ptr<Rectangle> rectangle = std::make_shared<Rectangle>();
	rectangle->SetColor(glm::vec3(1.0f, 0.0f, 0.0f)); 
	rectangle->Transform->Translate2D(glm::vec2(cellWidth * 0.5f, cellHeight * 0.5f));
	rectangle->Transform->Scale2D(glm::vec2(cellWidth, cellHeight));

	std::vector<glm::vec2> rectPosition;
	std::vector<float> values;
	
	for (size_t i = 0; i < gridResolution.x; i++)
	{
		for (size_t j = 0; j < gridResolution.y; j++)
		{
			values.push_back((float) (i*i + j*j) / (gridResolution.x * gridResolution.x + gridResolution.y * gridResolution.y));
			rectPosition.push_back(glm::vec2(cellWidth * (float) i , cellHeight * (float) j));
		}
	}

	// END CELLS

	// Particle representation
	std::shared_ptr<Circle> circle  = std::make_shared<Circle>();
	float circleRadius = 0.2f;
	circle->Transform->Scale2D(circleRadius);
	circle->SetColor(glm::vec3(1.0f));


	while (!window->ShouldClose()) {
		CORE_INFO(Time::GetDeltaTime());

		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		Renderer::BeginScene(camera);
		solver.update(0.005f);
		particleManager = solver.getParticleManager();

		Renderer::DrawShapeDuplicate(rectangle, rectPosition, values, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
		Renderer::DrawShapeDuplicate(gridLine, gridPosition, gridDirection, gridScale);

		//Renderer::DrawShapeDuplicate(circle, particleManager.pos); // Draw Border

		Renderer::EndScene();

		window->OnUpdate();
	}

	return 0;
}