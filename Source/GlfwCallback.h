#pragma once

struct GLFWwindow;

namespace GlfwCallback
{
	void ErrorCallback(const int aError, const char* aDescription);
	void FrameBufferSizeCallback(GLFWwindow* aWindow, const int aWidth, const int aHeight);
	void CursorCallback(GLFWwindow* aWindow, const double aX, const double aY);
	void ScrollCallback(GLFWwindow* aWindow, const double aXOffset, const double aYOffset);
}
