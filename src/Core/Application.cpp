#include "Application.h"

#include "Time.h"

#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Rendering/RendererCommand.h"

#include <functional>
#include <iostream>

// TEMP
#include "Scene/Circle.h"

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
	t_Controller = std::make_shared<CameraController>(camera, 0.1f);

	shared_ptr<Circle> circle = std::make_shared<Circle>();
	circle->Transform->Scale2D(10.0f);

  while (!m_Window->ShouldClose()) {
		//CORE_INFO(Time::GetDeltaTime());

		// TEMP : TODO PASS CAMERA SHARED PTR
		Renderer::BeginScene(*camera.get());

		RendererCommand::Clear();
		RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		Renderer::DrawShape(circle);

    // TODO : Update layers for events

    // TODO : Update layers for rendering (reverse order)

		Renderer::EndScene();
		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event &e)
{
	// TEMP
	t_Controller->OnEvent(e);

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(CORE_BIND_EVENT_METHOD(Application, OnKeyPressed));
	dispatcher.Dispatch<KeyReleasedEvent>(CORE_BIND_EVENT_METHOD(Application, OnKeyReleased));
	dispatcher.Dispatch<KeyTypedEvent>(CORE_BIND_EVENT_METHOD(Application, OnKeyTyped));

	dispatcher.Dispatch<MousePressedEvent>(CORE_BIND_EVENT_METHOD(Application, OnMousePressed));
	dispatcher.Dispatch<MouseReleasedEvent>(CORE_BIND_EVENT_METHOD(Application, OnMouseReleased));
	dispatcher.Dispatch<MouseTypedEvent>(CORE_BIND_EVENT_METHOD(Application, OnMouseTyped));
}

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
