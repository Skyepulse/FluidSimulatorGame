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


#include "Core/Rendering/Transform.h"
#include "Core/Rendering/Camera.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/RendererCommand.h"

#include "Core/Scene/Circle.h"

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Window.h"

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
	
	/* // SQUARE
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("src/shader.vertex", "src/shader.fragment");

	std::vector<float> vertices = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	};
	std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
	BufferLayout layout({
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float2, "a_TexCoords"},
	});
	vb->SetLayout(layout);

	std::vector<uint32_t> index = {
		0, 1, 2,
		2, 3, 0
	};

	std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(index.data(), index.size());

	VertexArray va{};
	va.AddVertexBuffer(vb);
	va.SetIndexBuffer(ib);

	// END SQUARE */

	// CIRCLE
	Circle circle;
	std::vector<glm::vec2> positions = { 
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 2.0f),
		glm::vec2(1.0f, 3.0f),
		glm::vec2(2.0f, 1.0f),
		glm::vec2(2.0f, 2.0f),
		glm::vec2(2.0f, 3.0f),
		glm::vec2(3.0f, 1.0f),
		glm::vec2(3.0f, 2.0f),
		glm::vec2(3.0f, 3.0f),
	};

	// END CIRCLE

	Camera camera(0.0f, 12.0f, 0.0f, 9.0f);

	// TEXTURE TEST
	Texture2D tex("src/texture.jpeg");

	while (!window->ShouldClose()) {
		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		Renderer::BeginScene(camera);

		Renderer::DrawCircleDuplicate(positions, circle);

		Renderer::EndScene();

		window->OnUpdate();
	}

	return 0;
}