#include "App.h"

#include <filesystem>

#include "LogUtility.h"
#include "RenderMate.h"

App::App()
	: myRenderMate(nullptr)
	, myShouldDestroy(false)
{
}

App::~App()
{
	delete myRenderMate;
}

void App::Initialize()
{
#if _WIN32
	const char* bits = "x86";
#elif _WIN64
	const char* bits = "x64";
#endif

	LogUtility::PrintMessage(LogUtility::LogCategory::Core, "Windows %s", bits);
	LogUtility::PrintMessage(LogUtility::LogCategory::Core, "Current working directory: %s", std::filesystem::current_path().string().c_str());

	myRenderMate = new RenderMate();
	myRenderMate->Initialize();
}

void App::Run()
{
	auto previousTime = std::chrono::high_resolution_clock::now();

	while (!myShouldDestroy)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = currentTime - previousTime;
		const float deltaTime = std::chrono::duration<float>(elapsedTime).count();
		previousTime = currentTime;

		myRenderMate->Update(deltaTime);

		myShouldDestroy = myRenderMate->ShouldClose();
	}
}

void App::Destroy() const
{
	myRenderMate->Destroy();
}
