#pragma once

struct Camera;

class InputManager
{
public:
	InputManager();

	void OnCursorAction(double aX, double aY);
	void OnScrollAction(double aXOffset, double aYOffset);

private:
	float myLastX;
	float myLastY;
	bool myIsFirstCursor;
};
