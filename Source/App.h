#pragma once

class Window;
class InputManager;
struct Camera;

class App
{
public:
	static App& GetInstance()
	{
		static App instance;
		return instance;
	}

	App(App const&) = delete;
	void operator=(App const&) = delete;

	void Initialize();
	void Update(const float aDeltaTime);

	Window& GetWindow() { return *myWindow; }
	InputManager& GetInputManager() { return *myInputManager; }
	Camera& GetCamera() { return *myCamera; }

private:
	App();
	~App();

	Window* myWindow;
	InputManager* myInputManager;
	Camera* myCamera;
};
