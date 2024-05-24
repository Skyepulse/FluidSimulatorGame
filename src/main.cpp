
#include "Core/Application.h"

Application* Application::Create() { return new Application(); }

int main() 
{
	Application* application = Application::Create();
	application->Start();
	delete application;
}