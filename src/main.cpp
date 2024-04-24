#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <string>
#include <iostream>

#include "OpenGL/Context.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Buffer.h"
#include "OpenGL/VertexArray.h"

#include "Core/Log.h"

int main() {
	Logger::Init();
	// ---------------------------------------------------------------------------
	//																	GLFW INIT
	// ---------------------------------------------------------------------------

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "GlassOverFlow", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	
	// ---------------------------------------------------------------------------
	//																	GLAD INIT
	// ---------------------------------------------------------------------------

	OpenGLContext context(window);
	context.Init();

	// ---------------------------------------------------------------------------
	//																	DATA INIT
	// ---------------------------------------------------------------------------
	
	Shader shader("src/shader.vertex");

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

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		va.Bind();
		shader.Bind();

		glDrawElements(GL_TRIANGLES, va.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

		glfwPollEvents();
		context.SwapBuffers();
	}

	return 0;
}