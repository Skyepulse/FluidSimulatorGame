#include "Application.h"

#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Rendering/RendererCommand.h"

#include "Layer.h"

#include "Log.h"

#include <functional>
#include <iostream>

Application* Application::s_Instance;

Application::Application()
{
 	Logger::Init();

	WindowProperties windowProps;
	windowProps.Width = 500;
	windowProps.Height = 1000;

	// We bind a non-static method, so we have to pass this as argument of the OnEvent method
	// The std::placeholders::_1 specify that if eventCallback(e) is executed, OnEvent(this, e) is executed
	// So the 'this' OnEvent method 

	// The callback is used to specify what function is called when the window received an event
	EventCallback eventCallback = CORE_BIND_EVENT_METHOD(Application, OnEvent);

	m_Window = std::make_shared<Window>(windowProps, eventCallback);
	m_Window->EnableVSync(true);

	userInterface.init(m_Window->Get());

  // TEMP A CHANGER
	// Enable Blending
	RendererCommand::EnableBlending(true);
}

Application::~Application()
{
}

void Application::Start()
{
	CORE_TRACE("Applications started")

	// TEMP ?
  	std::shared_ptr<Camera> camera = std::make_shared<Camera>(0.0f, 36.0f, 0.0f, 72.0f);
	t_Controller = std::make_shared<CameraController>(camera, 0.1f);

	while (!m_Window->ShouldClose()) 
	{
		// TEMP : TODO PASS CAMERA SHARED PTR
		Renderer::BeginScene(*camera.get());

		userInterface.newFrame();

		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		for(auto& layer : m_Layers)
			layer->Update();

		userInterface.show();

		Renderer::EndScene();
		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event &e)
{
	for(auto it = m_Layers.end() - 1; it != m_Layers.begin(); it--)
		(*it)->OnEvent(e);

	// TEMP ?
	if(t_Controller->OnEvent(e))
		return;
}