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
	windowProps.Width = 1080.0f;
	windowProps.Height = 720.0f;

	// We bind a non-static method, so we have to pass this as argument of the OnEvent method
	// The std::placeholders::_1 specify that if eventCallback(e) is executed, OnEvent(this, e) is executed
	// So the 'this' OnEvent method 

	// The callback is used to specify what function is called when the window received an event
	EventCallback eventCallback = CORE_BIND_EVENT_METHOD(Application, OnEvent);

	m_Window = std::make_shared<Window>(windowProps, eventCallback);
	m_Window->EnableVSync(true);

	userInterface = std::make_shared<UserInterface>();
	userInterface->init(m_Window.get());

	// TEMP A CHANGER
	// Enable Blending
	RendererCommand::EnableBlending(true);

	backgroundTex = std::make_shared<Texture2D>("src/data/bg.png");
	background = std::make_shared<Rectangle>();
	background->SetTexture(backgroundTex.get());
}

Application::~Application()
{
}

void Application::setBgSize(){
	background->Transform->SetPosition2D(m_Controller->getCenterBound());
	glm::vec2 size = glm::vec2(4401.0/2466.0, 1)*(2.0f*m_Controller->getZoomLevel())*1.05f;
	if (m_Controller->getAspectRatio() > 4401.0/2466.0) size *= m_Controller->getAspectRatio() / (4401.0/2466.0);
	background->Transform->SetSize(size);
}

void Application::Start()
{
	CORE_TRACE("Applications started")

	// TEMP ?
	m_Controller = std::make_shared<CameraController>(1080.0f / 720.0f, 35.99162f, 2.0f);
	setBgSize();

	while (!m_Window->ShouldClose()) 
	{
		if (!m_Minimized)
		{
			Renderer::BeginScene(m_Controller->GetCamera());

			userInterface->newFrame();

			RendererCommand::Clear();
			RendererCommand::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

			Renderer::DrawShape(background);

			for (auto& layer : m_Layers)
				layer->Update();

			userInterface->show();

			Renderer::EndScene();
		}

		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizedEvent>(CORE_BIND_EVENT_METHOD(Application, OnWindowResized));

	for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
		(*it)->OnEvent(e);

	if(!m_Minimized)
		userInterface->onEvent(e);

	if (e.GetEventType() == EventType::WindowResized) {
		setBgSize();
    }

	// TEMP ?
	if(m_Controller->OnEvent(e))
		return;
}

bool Application::OnWindowResized(WindowResizedEvent& e)
{
	CORE_DEBUG("REsized event")
	if (e.GetSize().x == 0 || e.GetSize().y == 0)
	{
		m_Minimized = true;
		return false;
	}

	setBgSize();

	m_Minimized = false;
	return false;
}
