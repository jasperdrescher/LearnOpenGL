#pragma once

struct Camera;

class InputManager
{
public:
	static InputManager& GetInstance()
	{
		static InputManager instance;
		return instance;
	}

	InputManager(InputManager const&) = delete;
	void operator=(InputManager const&) = delete;

	void AssignCamera(Camera* aCamera);
	void OnCursorAction(double aX, double aY);
	void OnScrollAction(double aXOffset, double aYOffset);

private:
	InputManager();
	~InputManager() {}

	Camera* myCamera;
	float myLastX;
	float myLastY;
	bool myIsFirstCursor;
};
