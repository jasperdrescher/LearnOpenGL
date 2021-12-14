#include "App.h"

#include "Camera.h"
#include "InputManager.h"
#include "Window.h"

App::App()
	: myWindow(nullptr)
	, myInputManager(nullptr)
	, myCamera(nullptr)
{}

App::~App()
{
	delete myCamera;
	delete myInputManager;
	delete myWindow;
}

void App::Initialize()
{
	myWindow = new Window();
	myInputManager = new InputManager();
	myCamera = new Camera();
}

void App::Update(const float aDeltaTime)
{
	myWindow->Update(aDeltaTime);
}
