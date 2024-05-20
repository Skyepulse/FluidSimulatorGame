#include "Application.h"

#include "Time.h"
#include "Log.h"

#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Rendering/RendererCommand.h"

Application::Application()
{
 	Logger::Init();

	std::shared_ptr<WindowProperties> windowProperties = std::make_shared<WindowProperties>();
	windowProperties->Width = 800;
	windowProperties->Height = 600;
	
	m_Window = std::make_shared<Window>(windowProperties);
	m_Window->EnableVSync(true);

  // TEMP A CHANGER
	// Enable Blending
	RendererCommand::EnableBlending(true);
}

Application::~Application()
{
}

void Application::Start()
{
  CORE_TRACE("Applications started");
  Camera camera(0.0f, 48.0f, 0.0f, 36.0f); // Multiply by h

  while (!m_Window->ShouldClose()) {
		CORE_INFO(Time::GetDeltaTime());

		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // TODO : Update layers for events

    // TODO : Update layers for rendering (reverse order)

		Renderer::EndScene();
		m_Window->OnUpdate();
	}
}
