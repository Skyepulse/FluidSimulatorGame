
#include "Game/GameApplication.h"

int main()
{
	Application* application = Application::Create();
	application->Start();
	delete application;
}