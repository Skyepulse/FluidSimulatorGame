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

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Window.h"

int main() {
	// Init of logger
	Logger::Init();
	
	// ---------------------------------------------------------------------------
	//															GLFW / GLAD INIT
	// ---------------------------------------------------------------------------

	std::shared_ptr<Window> window = std::make_shared<Window>();
	
	// TEMP A CHANGER
	// Enable Blending
	RendererCommand::EnableBlending(true);

	// ---------------------------------------------------------------------------
	//																	DATA INIT
	// ---------------------------------------------------------------------------
	
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

	// TEST
	Transform2D transform;
	transform.Translate(glm::vec3(1.0f, 1.0f, 0.0f));
	CORE_DEBUG("Model Matrix : {}", glm::to_string(transform.GetModelMatrix()))
	Camera camera(0.0f, 12.0f, 0.0f, 9.0f);

	// TEXTURE TEST
	Texture2D tex("src/texture.jpeg");

	while (!window->ShouldClose()) {
		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		Renderer::BeginScene(camera);

		Renderer::Draw(transform, shader, tex, va);

		Renderer::EndScene();

		window->OnUpdate();
	}

	return 0;
}