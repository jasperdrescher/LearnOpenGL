#include "GlfwCallback.h"

#include "App.h"
#include "InputManager.h"
#include "Window.h"

#include <stdio.h>

void GlfwCallback::ErrorCallback(const int aError, const char* aDescription)
{
	printf("%i %s\n", aError, aDescription);
}

void GlfwCallback::FrameBufferSizeCallback(GLFWwindow* /*aWindow*/, const int aWidth, const int aHeight)
{
	App::GetInstance().GetWindow().OnFrameBufferSizeChange(aWidth, aHeight);
}

void GlfwCallback::CursorCallback(GLFWwindow* /*aWindow*/, const double aX, const double aY)
{
	App::GetInstance().GetInputManager().OnCursorAction(aX, aY);
}

void GlfwCallback::ScrollCallback(GLFWwindow* /*aWindow*/, const double aXOffset, const double aYOffset)
{
	App::GetInstance().GetInputManager().OnScrollAction(aXOffset, aYOffset);
}
