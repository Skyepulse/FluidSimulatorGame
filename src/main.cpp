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

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Window.h"

int main() {
	// Init of logger
	Logger::Init();

	// Logger test
	Logger::Test();

	// Assert use example stop the program if not satisfied
	//CORE_ASSERT(1 != 2, "1 is equal to 2");
	//CORE_ASSERT(1 == 2, "1 is not equal to 2");

	// Example :
	// For engine related trace/debug/info/warning/error/critical
	CORE_TRACE("Rendering took {}ms and raised {} warnings", 5, 1518);

	// For game related trace/debug/info/warning/error/critical
	TRACE("Successfully initialized level in {}ms with {} warnings", 5, 145);
	
	// ---------------------------------------------------------------------------
	//																	GLFW INIT
	// ---------------------------------------------------------------------------

	std::shared_ptr<Window> window = std::make_shared<Window>();
	
	// ---------------------------------------------------------------------------
	//																	GLAD INIT
	// ---------------------------------------------------------------------------

	OpenGLContext context(window);
	context.Init();

	// ---------------------------------------------------------------------------
	//																	DATA INIT
	// ---------------------------------------------------------------------------
	
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("src/shader.vertex", "src/shader.fragment");

	std::vector<float> vertices = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f,
	};
	std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
	BufferLayout layout({
		{ShaderDataType::Float3, "a_Position"}
	});
	vb->SetLayout(layout);


	std::vector<uint32_t> index = {
		0, 1, 2,
	};

	std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(index.data(), index.size());

	VertexArray va{};
	va.AddVertexBuffer(vb);
	va.SetIndexBuffer(ib);

	// TEST
	glm::mat4 projMatrix = glm::mat4(1.0);
	CORE_DEBUG("VP Matrix : {}", glm::to_string(projMatrix))

	while (!window->ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		va.Bind();
		shader->Bind();
		shader->SetMat4("u_VPMatrix", projMatrix);

		glDrawElements(GL_TRIANGLES, va.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

		window->PollEvents();
		context.SwapBuffers();
	}

	return 0;
}