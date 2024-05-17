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

	// GRID
	std::shared_ptr<Shape> gridLine = std::make_shared<Line>();
	gridLine->SetColor(glm::vec3(1.0f));

	glm::ivec2 gridSize = glm::vec2(12, 9);
	glm::ivec2 gridResolution = glm::ivec2(12, 9);

	std::vector<glm::vec2> gridPosition;
	std::vector<glm::vec2> gridDirection;
	std::vector<glm::vec2> gridScale;

	float dx = (float)gridSize.x / ((float)gridResolution.x - 1.0f);
	float dy = (float)gridSize.y / ((float)gridResolution.y - 1.0f);

	/*gridPosition.push_back(glm::vec2(12.0f, 0.0f));
	gridDirection.push_back(glm::vec2(0.0f, 1.0f));
	gridScale.push_back(glm::vec2(gridSize.y, 1.0f));*/

	for (size_t i = 0; i < gridResolution.x; i++)
	{
		CORE_DEBUG("Position : {0}, {1}", dx * (float)i, 0.0f);
		gridPosition.push_back(glm::vec2(dx * (float)i, 0.0f));
		gridDirection.push_back(glm::vec2(0.0f, 1.0f));
		gridScale.push_back(glm::vec2(gridSize.y));
	}

	for (size_t i = 0; i < gridResolution.y; i++)
	{
		gridPosition.push_back(glm::vec2(0.0f, dy * (float)i));
		gridDirection.push_back(glm::vec2(1.0f, 0.0f));
		gridScale.push_back(glm::vec2(gridSize.x));
	}

	// END GRID

	// Solver
	Solver solver;
	solver.initSimulation(48.0f, 36.0f);
	ParticleManager particleManager;

	Camera camera(0.0f, 48.0f * solver.getH() * 2, 0.0f, 36.0f * solver.getH() * 2); // MUltiply by h

	/*
	// POSITION

	// Border Generation
	glm::vec2 cameraSize = camera.GetSize();
	glm::vec2 cameraOrigin = camera.GetOrigin();

	glm::ivec2 borderParticleCount = glm::vec2(60.0f, 40.0f);

	// Border positions generation
	std::vector<glm::vec2> borderPositions;

	glm::vec2 lbCorner = cameraOrigin;
	glm::vec2 rbCorner = cameraOrigin + glm::vec2(cameraSize.x, 0.0f);
	glm::vec2 ltCorner = cameraOrigin + glm::vec2(0.0f, cameraSize.y);
	glm::vec2 rtCorner = cameraOrigin + glm::vec2(cameraSize.x, cameraSize.y);

	// Horizontal borders
	dx = 1.0f / ((float) borderParticleCount.x - 1.0f);
	dy = 1.0f / ((float) borderParticleCount.y - 1.0f);

	glm::vec2 hiOffset = glm::vec2(0.0f, dy) * (ltCorner.y - lbCorner.y); // Horizontal inner border offset from outter border
	glm::vec2 viOffset = glm::vec2(dx, 0.0f) * (rbCorner.x - lbCorner.y); // Vertical inner border offset from outter border

	for (size_t i = 0; i < borderParticleCount.x ; i++)
	{
		float t = (float) i * dx;

		// Outter border
		borderPositions.push_back(rbCorner * (1.0f - t) + lbCorner * t);
		borderPositions.push_back(rtCorner * (1.0f - t) + ltCorner * t);

		// Inner border
		borderPositions.push_back(rbCorner * (1.0f - t) + lbCorner * t + hiOffset);
		borderPositions.push_back(rtCorner * (1.0f - t) + ltCorner * t - hiOffset);
	}

	// Vertical borders
	for (size_t i = 0; i < borderParticleCount.y ; i++)
	{
		float t = (float) i * dy;

		// Outter border
		borderPositions.push_back(rbCorner * (1.0f - t) + rtCorner * t);
		borderPositions.push_back(lbCorner * (1.0f - t) + ltCorner * t);

		// Inner border
		borderPositions.push_back(lbCorner * (1.0f - t) + ltCorner * t + viOffset);
		borderPositions.push_back(rbCorner * (1.0f - t) + rtCorner * t - viOffset);
	}

	// fluid position generation
	std::vector<glm::vec2> fluidPositions;
	glm::ivec2 fluidSize = glm::ivec2(20);
	glm::vec2 spacing = glm::vec2(0.2f);
	glm::vec2 fluidlbCorner = lbCorner + 3.0f * hiOffset + 3.0f * viOffset;

	for (size_t i = 0; i < fluidSize.x; i++)
	{
		for (size_t j = 0; j < fluidSize.y; j++)
		{
			fluidPositions.push_back(fluidlbCorner + glm::vec2(i, j) * spacing);
		}
	}

	// END POSITION
	*/

	// Physical values representations
	std::shared_ptr<Shape> line = std::make_shared<Line>();
	line->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));

	// Particle representation
	std::shared_ptr<Circle> circle = std::make_shared<Circle>();
	float circleRadius = 0.2f;
	circle->Transform->Scale2D(circleRadius);
	circle->SetColor(glm::vec3(1.0f));

	while (!window->ShouldClose()) {
		//CORE_INFO(Time::GetDeltaTime());

		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		Renderer::BeginScene(camera);

		solver.update(0.005f);

		Renderer::DrawShapeDuplicate(gridLine, gridPosition, gridDirection, gridScale);

		/*Renderer::DrawShapeDuplicate(circle, borderPositions); // Draw Border
		Renderer::DrawShapeDuplicate(circle, fluidPositions); // Draw fluid

		Renderer::DrawShapeDuplicate(line, borderPositions); // Draw Border
		Renderer::DrawShapeDuplicate(line, fluidPositions); // Draw fluid*/

		particleManager = solver.getParticleManager();

		Renderer::DrawShapeDuplicate(particleManager.pos, circle); // Draw Border

		Renderer::EndScene();

		window->OnUpdate();
	}

	return 0;
}