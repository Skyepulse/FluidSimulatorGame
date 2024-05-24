#include "Application.h"

#include "Time.h"

#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Rendering/RendererCommand.h"

#include <functional>
#include <iostream>

Application::Application()
{
 	Logger::Init();

	WindowProperties windowProps;
	windowProps.Width = 800;
	windowProps.Height = 600;

	// We bind a non-static method, so we have to pass this as argument of the OnEvent method
	// The std::placeholders::_1 specify that if eventCallback(e) is executed, OnEvent(this, e) is executed
	// So the 'this' OnEvent method 

	// The callback is used to specify what function is called when the window received an event
	EventCallback eventCallback = CORE_BIND_EVENT_METHOD(Application, OnEvent);

	m_Window = std::make_shared<Window>(windowProps, eventCallback);
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
  std::shared_ptr<Camera> camera = std::make_shared<Camera>(0.0f, 48.0f, 0.0f, 36.0f); // Multiply by h
	t_Controller = std::make_shared<CameraController>(camera, 0.3f);

	t_Game = std::make_shared<Game>();
	t_Game->Init();

  while (!m_Window->ShouldClose()) {
		//CORE_INFO(Time::GetDeltaTime());

		// TEMP : TODO PASS CAMERA SHARED PTR
		Renderer::BeginScene(*camera.get());

		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// TEMP
		t_Game->Update();

    // TODO : Update layers for events

    // TODO : Update layers for rendering (reverse order)

		Renderer::EndScene();
		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event &e)
{
	// TEMP
	if(t_Game->OnEvent(e))
		return;

	if(t_Controller->OnEvent(e))
		return;
}

// DEBUG METHODS

bool Application::OnKeyPressed(KeyPressedEvent &e)
{
	CORE_DEBUG(e.ToString())

	return true;
}

bool Application::OnKeyReleased(KeyReleasedEvent &e)
{
	CORE_DEBUG(e.ToString())

	return true;
}

bool Application::OnKeyTyped(KeyTypedEvent &e)
{
	CORE_DEBUG(e.ToString())

	return true;
}

bool Application::OnMousePressed(MousePressedEvent &e)
{
	CORE_DEBUG(e.ToString())

	return true;
}

bool Application::OnMouseReleased(MouseReleasedEvent &e)
{
	CORE_DEBUG(e.ToString())

	return true;
}

bool Application::OnMouseTyped(MouseTypedEvent & e)
{
	CORE_DEBUG(e.ToString())

	return true;
}
